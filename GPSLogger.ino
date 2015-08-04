#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

//pins of GPS and SD Card
#define GPS_RX 3
#define GPS_TX 4
#define SD_CS 2

//EEPROM offset
#define EEPROMPosition 48

//Output leds
#define ledSDErr 19
#define ledGPSStatus 17
#define ledLogging 15
#define ledSDWrite 14

//Intervals in ms
#define blinkDelay 500
#define blinkSDWriteDelay 150
#define writeInterval 1000


TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);
File dataFile;

unsigned long lastBlink = 0;
unsigned long lastWriteBlink = 0;
unsigned long lastWrite = 0;
unsigned int counter = 0;

boolean blinkGPSStatus = LOW;
boolean blinkSDErr = LOW;
boolean blinkLogging = LOW;
boolean blinkSDWrite = LOW;

char filename[8];

void setup() {
	Serial.begin(115200);
	ss.begin(9600);
	delay(20);

	Serial.println("IO Init...");

	//LEDs = output
	pinMode(ledSDErr, OUTPUT);
	pinMode(ledGPSStatus, OUTPUT);
	pinMode(ledLogging, OUTPUT);
	pinMode(ledSDWrite, OUTPUT);

	//blink them
	digitalWrite(ledSDErr, HIGH);
	digitalWrite(ledGPSStatus, HIGH);
	digitalWrite(ledLogging, HIGH);
	digitalWrite(ledSDWrite, HIGH);

	delay(500);
	//shut them down
	digitalWrite(ledSDErr, LOW);
	digitalWrite(ledGPSStatus, LOW);
	digitalWrite(ledLogging, LOW);
	digitalWrite(ledSDWrite, LOW);

	//load last filename from EEPROM
	Serial.println("Loading fileid from EEPROM");
	counter = EEPROMReadInt(EEPROMPosition);
	//increment
	counter++;
	Serial.print("Loaded: "); 
	sprintf(filename, "%07d", counter);
	Serial.print(filename);
	Serial.println(".log");
	//write back
	Serial.println("Writing new counter status");
	EEPROMWriteInt(EEPROMPosition, counter);

	//Open SD Card, wait until it's connected
	Serial.println("Opening SD Card...");
	blinkSDErr = HIGH;
	digitalWrite(ledSDErr, blinkSDErr);
	while (!SD.begin(SD_CS)){
		Serial.println("Failed...");
		delay(1000);
	}
	delay(100);
	//hurray, it's connected, can continue
	blinkSDErr = LOW;
	digitalWrite(ledSDErr, blinkSDErr);
	Serial.println("SD Opened successfully!");

}

void loop() {
	//what about blinking some LEDs?
	if ((lastBlink + blinkDelay) < millis()){
		lastBlink = millis();
		digitalWrite(ledGPSStatus, blinkGPSStatus);
		blinkGPSStatus = LOW;

		digitalWrite(ledSDErr, blinkSDErr);

		digitalWrite(ledLogging, blinkLogging);
		blinkLogging = LOW;
	}

	//SD Write LED
	if ((lastWriteBlink + blinkSDWriteDelay) < millis()){
		digitalWrite(ledSDWrite, blinkSDWrite);
		blinkSDWrite = LOW;
	}

	while (ss.available()) {
		if (gps.encode(ss.read())) {
			blinkGPSStatus = HIGH;
			Serial.println("Parse....");
		}
	}

	if (blinkGPSStatus == HIGH){
		if ((lastWrite + writeInterval) < millis()){
			lastWrite = millis();
			dataFile = SD.open((String)filename + ".log", FILE_WRITE);
			//Is file opened
			if (dataFile) {
				//If there was error last time, not, there isn't
				blinkSDErr = LOW;
				if (gps.location.isValid()) {
					blinkLogging = HIGH;
				}
				blinkSDWrite = HIGH;
				digitalWrite(ledSDWrite, blinkSDWrite);
				//CSV structure
				// 0 year
				column(dataFile, (String)gps.date.year());
				// 1 month
				column(dataFile, (String)gps.date.month());
				// 2 day
				column(dataFile, (String)gps.date.day());
				// 3 hour
				column(dataFile, (String)gps.time.hour());
				// 4 minute
				column(dataFile, (String)gps.time.minute());
				// 5 second
				column(dataFile, (String)gps.time.second());
				// 6 location validity
				column(dataFile, (String)gps.location.isValid());
				// 7 LATITUDE
				column(dataFile, String(gps.location.lat(), 7));
				// 8 LONGTITUDE
				column(dataFile, String(gps.location.lng(), 7));
				// 9 ALTITUDE 
				column(dataFile, (String)gps.altitude.meters());
				// 10 number of satelites fixed
				column(dataFile, (String)gps.satellites.value());
				

				//end of line
				row(dataFile);

				dataFile.close();				
			} 
			//what if file didn't open?
			else {
				//blink led
				blinkSDErr = HIGH;
				Serial.println("SD Write Error!!!");
			}
		}
	}
}

// Write Int into EEPROM (2bytes)
void EEPROMWriteInt(int address, unsigned int value){
	byte a = value / 256;
	byte b = value % 256;
	EEPROM.write(address, a);
	EEPROM.write(address + 1, b);
}

// Read Int from EEPROM (2 bytes)
unsigned int EEPROMReadInt(int address){
	byte a = EEPROM.read(address);
	byte b = EEPROM.read(address + 1);
	return (a*256 + b);
}

// Write CSV column to SD and into console
void column( File df, String value){
	df.print(value);
	df.print(",");
	Serial.print(value);
	Serial.print(",");
}

//end line of csv
void row(File df){
	df.println();
	Serial.println();
}

