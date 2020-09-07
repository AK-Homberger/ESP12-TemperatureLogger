# ESP12 Temperature Logger
This is a battery powered tempeature logger with an ESP12 (ESP8266 derivate) and DS3231 Real Time Clock.

The logger is using the ESP12 deep sleep function to save power.
The internal wake up function of the ESP12 is used. The maximum sleep time is about 3 hours due to internal limitations.
The RTC time is read after reboot an the time is used to calculate the necessary next sleeptime.

To use the DS3231 you have to install the Adafruit RTClib with the IDE Library Manager.

The temperature is measured with the DS3231 internal sensor and stored in internal flash memory of ESP12 (up to 4K is possible).

One mesuring cycle takes only 50 ms. The used battery CR123a with 1500 mAh should work months/years.
In deep sleep the ESP12 takes about 20 uA. During operation about 50 mA (with RF components disabled).
During the measuring cycle the LED is on. The whole programm is in the setup() section of the code, loop() is empty.

You can adjust the following lines in the code due to your needs:

#define LogInterval 3600*12 // 12 hours. Two logs per day. Change to desired value!!!
#define ENABLE_DEBUG_LOG 1  // 0 = Disable, 1 = Enable debug log info via Serial
#define EepromSize 512      // > 500 entries of temperature storage (possible EepromSize up to 4096 for ESP12E)
#define TIME_SET 0          // Set to 1 for time set of RTC (only first time and after DS3231 battery replacement)

![Schematic](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/ESP12TempLoggerRTC.png)

![Box](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/IMG_1369.jpg)

![Wiring](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/IMG_1364.jpg)

# Programming
To program the ESP12 an external USB to serial adapter is necessary (3 Volt logic compatibel).

For programming you have to connect RX, TX and GND (connect RX to TX and TX to RX). Power for the ESP12 should come from battery.
Switch FLASH switch to GND and press RST button. Then start upload in the IDE.
For normal operation FLASH switch has to be open.

# Time setting

Just in case you are using a DS3231 for the first time, you have to set the current time to the device. You can either use one of the examples for the Adafruit RTClib or you can use the current logger program. Just set the TIME_SET constant to 1 and upload the sketch and restart the ESP12. Then change the constant back to 0 and upload again.

# Show data
The USB Serial adapter is also necessary to read the stored data.
Switch selction switch to SHOW state (D6 to GND) and press RST button. The currently stored log data will be sent via the serial interface to the USB Serial adapter and is shown on the PC. You can use the IDE Serial Monitor tool to show the data. Set the baud rate to 74880. This is the standard baud rate for ESP12 startup. 
You can show data at any time. The logging is not disturbed by showing the data. You can also set ENABLE_DEBUG_LOG to 1 to get additional information.

16:24:48.968 -> Counter: 1 
16:24:48.968 -> First Time:    Mon 2020-09-07 11:31:01 
16:24:48.968 -> Now Time:      Mon 2020-09-07 16:23:59 
16:24:48.968 -> Next Log Time: Mon 2020-09-07 23:31:01 
16:24:48.968 -> Difference: 25622 
16:24:48.968 -> Sleep Time: 10800 
16:24:48.968 -> 
16:24:48.968 -> Counter: 1 
16:24:48.968 -> Show stored data
16:24:48.968 -> ----------------
16:24:48.968 -> Mon 2020-09-07 11:31:01 23.25°C 
16:24:48.968 -> 
16:24:48.968 -> 

# Reset log and start new logging
To reset the internal counter and start a new log cycle, switch selection switch to RESET state (D5 to GND) and press RST button. The LED will blink three times to show success. Then switch RESET back to (SHOW or neutral).
The new log cycle will start after 10 seconds with a new wake up cycle.


# Updates

07.09.2020 - Version 1.0: Initial version.


