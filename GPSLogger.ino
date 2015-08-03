#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

//Výstupní piny
#define ledSDErr 19
#define ledGPSStatus 17
#define ledLogging 15
#define ledSDWrite 14

//Pauza mezi blikancy
#define blinkDelay 500


TinyGPSPlus gps;
SoftwareSerial ss(3, 4);

unsigned long lastBlink = 0;
unsigned int counter = 0;

boolean blinkGPSStatus = LOW;

void setup() {
	Serial.begin(115200);
	ss.begin(9600);
	delay(20);

	Serial.println("IO Init...");

	pinMode(4, INPUT);
	pinMode(5,INPUT);
	pinMode(6, INPUT);
	pinMode(3, INPUT);

	//ledky jsou výstupní
	pinMode(ledSDErr, OUTPUT);
	pinMode(ledGPSStatus, OUTPUT);
	pinMode(ledLogging, OUTPUT);
	pinMode(ledSDWrite, OUTPUT);

	//tak je rožnu
	digitalWrite(ledSDErr, HIGH);
	digitalWrite(ledGPSStatus, HIGH);
	digitalWrite(ledLogging, HIGH);
	digitalWrite(ledSDWrite, HIGH);

	delay(500);
	//to snad stačilo
	digitalWrite(ledSDErr, LOW);
	digitalWrite(ledGPSStatus, LOW);
	digitalWrite(ledLogging, LOW);
	digitalWrite(ledSDWrite, LOW);

	//načtu data z eeprom
	Serial.println("Loading fileid from EEPROM");
	counter = EEPROMReadlong(12);
	counter++;
	Serial.print("Loaded: "); 
	Serial.println((String)counter);
	Serial.println("Writing new counter status");
	EEPROMWritelong(12, counter);



	//otevřu SD kartu

}

void loop() {
	//nemám blikat status diodou ledky? 
	if ((lastBlink + blinkDelay) < millis()){
		lastBlink = millis();
		digitalWrite(ledGPSStatus, blinkGPSStatus);
		blinkGPSStatus = LOW;
	}

	while (ss.available()) {
		if (gps.encode(ss.read())) {
			blinkGPSStatus = HIGH;
			Serial.println("Parse....");
			Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
			Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
			Serial.print("ALT=");  Serial.println(gps.altitude.meters());
			Serial.print("SATS="); Serial.println(gps.satellites.value());
			Serial.print("STAMP="); Serial.print(gps.date.day()); Serial.print('.');
			Serial.print(gps.date.month()); Serial.print('.');
			Serial.print(gps.date.year()); Serial.print(' ');
			Serial.print(gps.time.hour()); Serial.print(':');
			Serial.print(gps.time.minute()); Serial.print(':');
			Serial.println(gps.time.second());
		}
	}

}

// uložení int do EEProm
void EEPROMWriteInt(int address, unsigned int value){
	byte a = value / 256;
	byte b = value % 256;
	EEPROM.write(address, a);
	EEPROM.write(address + 1, b);
}

// načtení int z EEPROM
unsigned int EEPROMReadInt(int address){
	byte a = EEPROM.read(address);
	byte b = EEPROM.read(address + 1);
	return (a*256 + b);
}


