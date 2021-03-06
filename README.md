# ESP12 Temperature Logger
This is a battery powered tempeature logger with an ESP12 (ESP8266 derivate) and DS3231 Real Time Clock.

The logger is using the ESP12 deep sleep function to save power (connection between GPIO16 and RST necessary).
The internal wake up function of the ESP12 is used. The maximum sleep time is about 3 hours due to internal limitations.
The RTC time is read after reboot and the time is used to calculate the necessary next sleeptime. The DS3231 VCC pin (and the status LED) is powered from the GPIO pin D7. During Deep Sleep the DS3231 is using its own backup battery.

The temperature is measured with the DS3231 internal sensor and stored in internal flash memory of ESP12 (up to 4KB is possible).

One measuring cycle takes only 50 ms. The used battery CR123a with 1500 mAh should work for months/years.
In deep sleep the ESP12 takes about 20 µA. During operation about 50 mA (with RF components disabled).
During the measuring cycle the LED is on. The whole programm is in the setup() section of the code, loop() is empty.

You can adjust the following lines in the code due to your needs:

```cpp
#define LogInterval 3600*12 // 12 hours. Two logs per day. Change to desired value!!!
#define ENABLE_DEBUG_LOG 1  // 0 = Disable, 1 = Enable debug log info via Serial
#define EepromSize 512      // > 500 entries of temperature storage (possible EepromSize up to 4096 for ESP12E)
#define TIME_SET 0          // Set to 1 for time set of RTC (only first time and after DS3231 battery replacement)
```

![Schematic](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/ESP12TempLoggerRTC.png)

![Box](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/IMG_1369.jpg)

![Wiring](https://github.com/AK-Homberger/ESP12-TemperatureLogger/blob/master/IMG_1364.jpg)

## Programming
* To use the DS3231 you have to install the Adafruit RTClib with the IDE Library Manager.
* Set board to LOLIN (WEMOS) D1 R2 & mini - This board has ESP12 4MB and definitions for D1 - D12 pins
* To program the ESP12 an external USB to serial adapter is necessary (3 Volt logic compatibel). For logging the USB Serial adapter is not necessary.

For programming you have to connect RX, TX and GND (connect RX to TX and TX to RX). Power for the ESP12 should come from battery.
Switch FLASH switch to GND and press RST button. Then start upload in the IDE.
For normal operation FLASH switch has to be open.

## Time setting

Just in case you are using a DS3231 for the first time, you have to set the current time to the device. You can either use one of the examples for the Adafruit RTClib or you can use the current logger program. Just set the TIME_SET constant to 1 and upload the sketch and restart the ESP12. Then change the constant back to 0 and upload again.

## Show data
The USB Serial adapter is also necessary to read the stored data.
Switch selection switch to SHOW state (D6 to GND) and press RST button. The currently stored log data will be sent via the serial interface to the USB Serial adapter and is shown on the PC. You can use the IDE Serial Monitor tool to show the data. Set the baud rate to 74880. This is the standard baud rate for ESP12 startup. 
You can show data at any time. The logging is not disturbed by showing the data. You can also set ENABLE_DEBUG_LOG to 1 to get additional information.

```cpp
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
```
## Reset log and start new logging
To reset the internal counter and start a new log cycle, switch selection switch to RESET state (D5 to GND) and press RST button. The LED will blink three times to show success. Then switch RESET back to (SHOW or neutral).
The new log cycle will start after 10 seconds with a new wake up cycle.

## Partlist:

- ESP12 [Link](https://www.reichelt.de/de/en/index.html?ACTION=446&LA=3&nbc=1&q=esp12)
- Resistor 1 KOhm [Link](https://www.reichelt.de/de/en/carbon-film-resistor-1-4-w-5-1-0-kilo-ohms-1-4w-1-0k-p1315.html?&trstct=pos_1&nbc=1)
- Push button [Link](https://www.reichelt.de/miniatur-drucktaster-0-5a-24vac-1x-ein-rt-t-250a-rt-p31772.html?&trstct=pol_12&nbc=1)
- 2 * Toggle switch [Link](https://www.reichelt.de/de/en/miniature-toggle-switch-1x-on-off-on-rnd-210-00448-p240580.html?GROUPID=7584&START=0&OFFSET=16&SID=96Xk5YJngRlij1C8dm7WFa8cc43c9fd0145a715a7ea5bf81fdb75&LANGUAGE=EN&&r=1)
- Battery 123A [Link](https://www.reichelt.de/de/en/varta-photo-3-volt-1430-mah-17x34-5mm-varta-cr-123a-p7352.html?search=123a&&r=1)
- Battery holder [Link](https://www.reichelt.de/de/en/battery-holder-for-2-3a-cr-123--halter-2-3a-p44605.html?search=battery+holder+123&&r=1)
- LED [Link](https://www.reichelt.de/de/en/led-3-mm-red-270-mcd-80--rnd-135-00118-p263789.html?&trstct=pos_10&nbc=1)
- DS3231 [Link](https://www.reichelt.de/de/en/developer-boards-precision-real-time-clock-ds3231-debo-rtc-uhr2-p235506.html?&trstct=pos_1&nbc=1) or [Link](https://www.amazon.de/AZDelivery-RTC-Batterie-inklusive-Arduino/dp/B01M2B7HQB/ref=asc_df_B01M2B7HQB/?tag=googshopde-21&linkCode=df0&hvadid=310594507569&hvpos=&hvnetw=g&hvrand=7596897232631074358&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9043858&hvtargid=pla-378671262376&psc=1&th=1&psc=1&tag=&ref=&adgrpid=64737915114&hvpone=&hvptwo=&hvadid=310594507569&hvpos=&hvnetw=g&hvrand=7596897232631074358&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9043858&hvtargid=pla-378671262376)
- CR1220 (for DS3231 from Reichelt) [Link](https://www.reichelt.de/de/en/index.html?ACTION=446&LA=3&nbc=1&q=cr1220)
- Housing [Link](https://www.reichelt.de/de/en/small-plastic-housing-72-x-50-x-28-mm-geh-ks-28-p8160.html?GROUPID=7712&START=0&OFFSET=16&SID=96Xk5YJngRlij1C8dm7WFa8cc43c9fd0145a715a7ea5bf81fdb75&LANGUAGE=EN&&r=1)
- USB Programming Adapter [Link](https://www.reichelt.de/de/en/raspberry-pi-usb-ttl-interface-converter-ch340g-debo-usb-ttl-p266106.html?&trstct=pol_4&nbc=1) or [Link](https://www.amazon.de/tinxi%C2%AE-FT232RL-Converter-Adapter-Arduino/dp/B01C2P9GD2/ref=sr_1_17?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=FOANZTY2BP83&dchild=1&keywords=ftdi+usb+adapter&qid=1601370311&sprefix=ftdi%2Caps%2C186&sr=8-17)


## Updates

07.09.2020 - Version 1.0: Initial version.


