# ESP12 TemperatureLogger
This is battery powered tempeature logger with RTC DS3231.

The logger is using the ESP12 (ESP8266) deep sleep function to save power.
The internal wakup function of the ESP12 is used. The maximum sleep time is abut 3 hores due to internal limitations.
The RTC is read after reboot an the time is used to calculate the nexessary next sleeptime.

The temerature is measured with DS3231 internal sensor and stored in internal flash memory of ESP12 (up to 4K is possible).

One mesuring cycle takes only 50 ms. The used battery CR123a with 1500 mAH should work months/years.
In deep sleep the ESP12 takes about 20 uA. During operation about 50 mA (with RF components disabled).

# Programming
To program the ESP12 an external USB to serial adapter is necessary (3 Volt logic compatibel).

For programming you have to connect RX, TX and GND. Power for the ESP12 should come from battery.
Switch Programming switch to GND and press RSP button. the start upload in the IDE.
For normal operation Programm switch has to be open.

# Show data
The serial adapter is also necessary to read the stored data.
Switch selction switch to SHOW state (D6 to GND) an press RST button. The currently stored log date will be sent via the serial interfac to the USB Serial adapte and is shown on the PC. You can use the IDE Serial Monitor tool to show the data). Set the baud rate to 74880. This is the standard baud rate for ESP12 startup. 
You can show data at any time. The logging is not disturbed by showing the data.

# Reset log and start new logging










