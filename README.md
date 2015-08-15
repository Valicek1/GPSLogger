# GPSLogger

Simple GPS logger for Arduino. Tested with atmega328p (UNO Clone). Uses simple SD card and NEO-6M module for arduino. Logs into CSV file.

## Arduino Pinout

 - GPS TxD - Arduino RxD - pin 3 (changeable)
 - GPS RxD - Arduino TxD - pin 4 (changeable)
 - SD Card MOSI - Arduino pin 11
 - SD Card MISO - Arduino pin 12
 - SD Card CLK - Arduino pin 13
 - SD Card CS - Arduino pin 2 (changeable)

## Used Libraries
### Arduino Internal
 - SoftwareSerial
 - SPI
 - SD
 - EEPROM
### External
 - TinyGPS++ v0.94 (http://arduiniana.org/libraries/tinygpsplus/)

## Output data
 - SD/SD2/SDHC card, fat16/fat32 format, 8.3 file name convention
 - CSV file, with firmware adjustable interval (define in sketch)
 - named as xxxxxxx.log where xxxxxxx is number of logging (EEPROM stored)
 - columns (divided with ',' character)
   - year
   - month
   - day
   - hour
   - minute
   - second
   - location validity (0/1)
   - latitude
   - longtitude
   - altitude
   - number of fixed satelites

## Status Leds
 - SD Card opening error
 - GPS Data RX last second
 - GPS Data valid last second
 - SD Card write

## Prebuild Binaries
 - atmega328p (arduino uno compatible pinout)
 - 16Mhz XTAL

