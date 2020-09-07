/*
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
// Low power teperature logger with ESP12 module and DS3231 RTC module
// DS3231 is connected via I2C and has built in temperature sensor.
// With ESP deep sleep and battery CR123 (3 Volt) logging over months is possible

// Version 1.0, 07.09.2020, AK-Homberger

// Set board to LOLIN (WEMOS) D1 R2 & mini - This board has ESP12 4MB and definitions for D1 - D12 pins
// ESP12 wiring:

// Connect GPIO15 to GND
// Connect EN/CH to 3,3V
// Connect D0/GPIO16 with RST for ESP12 timer wakeup

// D1/GPIO5  = SCL DS3231
// D2/GPIO4  = SDA DS2331
// GND       = GND DS3231
// D7/GPIO13 = VCC DS3231 - RTC (and status LED) is powered via GPIO13 whwn ESP12 is running

// RST pin pushbutton to GND              - Push RST button for programming with IDE (GPIO0 must be LOW)
// D3/GPIO0 Programming switch            - Switch to GND and push RST button for programing with IDE
// D5/GPIO14 Counter Reset switch to GND  - Switch to GND and push RST button for counter reset and logging start
// D6/GPIO12 Show switch D6 to GND        - Switch to GND and push RST button to print log data via Serial (74880 baud)
// D7/GPIO13 Status LED                   - Connect status LED anode (longer pin) and cathode via 1K resistor to GND
// LED is ON while running (about 50 ms) and blinking twice for counter reset

// USB Serial adapter (set to 3,3 Volt logic level !!!!!)

// Connect ESP12 TX to USB Serial RX pin
// Connect ESP12 RX to USB Serial TX pin
// Connect ESP12 GND USB Serial GND
// Connect ESP12 3,3V with USB Serial 3,3V only if no CR123 battery is inserted!!!


#include <EEPROM.h>  // To use ESP12 internal flash memory to store logging data
#include <RTClib.h>  // Include Adafruit RTClib with library manager 
#include <time.h>    // For time calculations

//-------------------------------------------------------------------------------------------------------------
// Change these four values only !!!
#define LogInterval 3600*12 // 12 hours. Two logs per day. Change to desired value!!!
#define ENABLE_DEBUG_LOG 1  // 0 = Disable, 1 = Enable debug log info via Serial
#define EepromSize 512      // > 500 entries of temperature storage (possible EepromSize up to 4096 for ESP12E)
#define TIME_SET 0          // Set to 1 for time set of RTC (only first time and after DS3231 battery replacement)
//-------------------------------------------------------------------------------------------------------------

#define MaxSleepTime 10800  // Maximum Deep Sleep time for ESP12 is about three hours sleep (in seconds)
#define ACCURACY 10         // Define accuracy of logging (seconds) to avoid going to sleep for very short time  

RTC_DS3231 rtc;

void setup() {
  unsigned int counter;
  bool reset_counter;
  bool show_data;
  int i;
  float t;
  char date[80];
  DateTime Now;
  unsigned long Now_UNIX, SleepTime;
  unsigned long NextLogTime, FirstLogTime;
  long Difference;

  time_t rawtime;   // For time conversions
  struct tm  ts;

  pinMode(D5, INPUT_PULLUP);   // Reset counter switch
  pinMode(D6, INPUT_PULLUP);   // Show data switch
  pinMode(D7, OUTPUT);         // RTC and status LED power via pin D7 (about 3 mA)

  digitalWrite(D7, HIGH);      // Switch DS3231 RTC Power and Status LED on - Switched off in ESP12 Deep Sleep mode
  delay(5);                    // Give some time for RTC I2C startup

  // initializing the rtc
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    ESP.deepSleep(MaxSleepTime * 1e6, WAKE_RF_DISABLED);
  }

#if TIME_SET == 1  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // This will set the RTC time to PC time at upload
#endif

  // RTC DS3231 settings

  rtc.disable32K();
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);

  // Read switches
  reset_counter = digitalRead(D5);
  show_data = digitalRead(D6);

  Serial.begin(74880);

  Now = rtc.now();             // Get current time from RTC
  Now_UNIX = Now.unixtime();   // Convert to UNIX time (seconds since 1971)

  EEPROM.begin(EepromSize);

  // Read data from EEPROM

  counter = EEPROM.read(0) + (EEPROM.read(1) << 8);
  NextLogTime = EEPROM.read(2) + (EEPROM.read(3) << 8) + (EEPROM.read(4) << 16) + (EEPROM.read(5) << 24);
  FirstLogTime = EEPROM.read(6) + (EEPROM.read(7) << 8) + (EEPROM.read(8) << 16) + (EEPROM.read(9) << 24);
  Difference = NextLogTime - Now_UNIX;

#if ENABLE_DEBUG_LOG == 1

  Serial.printf("Counter: %d \n", counter);

  rawtime = FirstLogTime;
  ts = *localtime(&rawtime);
  strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", &ts);
  Serial.printf("First Time:    %s \n", date);

  rawtime = Now_UNIX;
  ts = *localtime(&rawtime);
  strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", &ts);
  Serial.printf("Now Time:      %s \n", date);

  rawtime = NextLogTime;
  ts = *localtime(&rawtime);
  strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", &ts);
  Serial.printf("Next Log Time: %s \n", date);

  Serial.printf("Difference: %d \n", Difference);

#endif

  if (Difference < ACCURACY) {   // No sleep for ACCURACY seconds difference, do logging
    NextLogTime = NextLogTime + LogInterval;   // Set next Log Time
    SleepTime = LogInterval;                   // Set next Sleep Time

#if ENABLE_DEBUG_LOG == 1
    rawtime = NextLogTime;
    ts = *localtime(&rawtime);
    strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", &ts);
    Serial.printf("New Next Log Time: %s \n", date);
#endif

  } else {
    SleepTime = Difference;     // Difference is next Sleep Time (is  > ACCURACY)
  }

  if (SleepTime > MaxSleepTime) SleepTime = MaxSleepTime;   // Check maximum Sleep Time
  if (SleepTime < ACCURACY) SleepTime = ACCURACY;           // Minimum is ACCURACY time

#if ENABLE_DEBUG_LOG == 1
  Serial.printf("Sleep Time: %d \n", SleepTime);
#endif

  if (!show_data) {    // Show data if switch is on (LOW)
    Serial.println();
    Serial.printf("Counter: %d \n", counter);
    Serial.println(F("Show stored data"));
    Serial.println(F("----------------"));

    for (i = 0; i < counter; i++) {
      rawtime = FirstLogTime + (i * LogInterval);   // Create timstamp from First Log Time and counter
      ts = *localtime(&rawtime);
      strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S ", &ts);  // Convert to readable time

      Serial.printf("%s", date);
      Serial.print((signed char)EEPROM.read(i + 10) / 4.0);
      Serial.println(F("°C "));
    }

    Serial.println();
    Serial.println();
  }

  if (!reset_counter) {   // Counter reset if switch is on (LOW)
    Serial.println(F("Reset counter."));
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);

    NextLogTime = Now_UNIX + 10 ; // Directly start with first temperature in 10 seconds
    SleepTime = 10;

#if ENABLE_DEBUG_LOG == 1
    Serial.printf("New Next Log Time: %d \n", NextLogTime);
    Serial.printf("Sleep Time: %d \n", SleepTime);
#endif

    EEPROM.write(2, NextLogTime & 0xff);        // Store next log time (which is the first log time now)
    EEPROM.write(3, (NextLogTime >> 8)  & 0xff);
    EEPROM.write(4, (NextLogTime >> 16) & 0xff);
    EEPROM.write(5, (NextLogTime >> 24) & 0xff);

    EEPROM.write(6, NextLogTime & 0xff);        // Store first log time (equal to next log time now)
    EEPROM.write(7, (NextLogTime >> 8)  & 0xff);
    EEPROM.write(8, (NextLogTime >> 16) & 0xff);
    EEPROM.write(9, (NextLogTime >> 24) & 0xff);

    EEPROM.commit();
    Blink(3);
    ESP.deepSleep(SleepTime * 1e6, WAKE_RF_DISABLED);
  }

  if (Difference < ACCURACY ) {       // Log temp data now
    t = rtc.getTemperature() - 1.3 ;  // Read temperature from RTC with optinal correction (-1.3 for my module)

#if ENABLE_DEBUG_LOG == 1
    Serial.printf("Store data: %2.2f °C\n", t);
#endif

    t = t * 4.0 ;         // Store about +/-30 °C with 0.25 °C accuracy in one byte
    if (t > 127) t = 127;
    if (t < -127) t = -127;

    EEPROM.write(counter + 10, (signed char) t);

    counter ++;
    if (counter > EepromSize - 11 ) counter--;   // Handle counter roll over, overwrite last entry

    EEPROM.write(0, counter & 0xff);
    EEPROM.write(1, (counter >> 8) & 0xff);

    EEPROM.write(2, NextLogTime & 0xff);
    EEPROM.write(3, (NextLogTime >> 8)  & 0xff);
    EEPROM.write(4, (NextLogTime >> 16) & 0xff);
    EEPROM.write(5, (NextLogTime >> 24) & 0xff);

    EEPROM.commit();
  }

  ESP.deepSleep(SleepTime * 1e6, WAKE_RF_DISABLED);
}



void Blink(int blink) {   // Blink status LED
  int i;

  for (i = 0; i < blink; i++) {
    digitalWrite(D7, LOW);
    delay(500);
    digitalWrite(D7, HIGH);
    delay(500);
  }
}



void loop() {   // We do nothing in loop()

}
