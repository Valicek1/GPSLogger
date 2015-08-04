#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

//piny GPS a karty
#define GPS_RX 3
#define GPS_TX 4
#define SD_CS 2

//eeprom
#define EEPROMPosition 48

//Výstupní piny
#define ledSDErr 19
#define ledGPSStatus 17
#define ledLogging 15
#define ledSDWrite 14

//Pauza mezi blikancy
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
	counter = EEPROMReadInt(EEPROMPosition);
	counter++;
	Serial.print("Loaded: "); 
	sprintf(filename, "%07d", counter);
	Serial.print(filename);
	Serial.println(".log");
	Serial.println("Writing new counter status");
	EEPROMWriteInt(EEPROMPosition, counter);

	//otevřu SD kartu
	Serial.println("Opening SD Card...");
	blinkSDErr = HIGH;
	digitalWrite(ledSDErr, blinkSDErr);
	while (!SD.begin(SD_CS)){
		Serial.println("Failed...");
		delay(1000);
	}
	delay(100);
	blinkSDErr = LOW;
	digitalWrite(ledSDErr, blinkSDErr);
	Serial.println("SD Opened successfully!");

}

void loop() {
	//nemám blikat status diodou ledky? 
	if ((lastBlink + blinkDelay) < millis()){
		lastBlink = millis();
		digitalWrite(ledGPSStatus, blinkGPSStatus);
		blinkGPSStatus = LOW;

		digitalWrite(ledSDErr, blinkSDErr);

		digitalWrite(ledLogging, blinkLogging);
		blinkLogging = LOW;
	}

	//ledka zapisu na SD
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
			//otevřel se soubor - zápis
			if (dataFile) {
				//zhastnu případnej error
				blinkSDErr = LOW;
				if (gps.location.isValid()) {
					blinkLogging = HIGH;
				}
				blinkSDWrite = HIGH;
				digitalWrite(ledSDWrite, blinkSDWrite);
				// 0 rok
				column(dataFile, (String)gps.date.year());
				// 1 měsíc
				column(dataFile, (String)gps.date.month());
				// 2 den
				column(dataFile, (String)gps.date.day());
				// 3 hodina
				column(dataFile, (String)gps.time.hour());
				// 4 minuta
				column(dataFile, (String)gps.time.minute());
				// 5 sekunda 
				column(dataFile, (String)gps.time.second());
				// 6 validita
				column(dataFile, (String)gps.location.isValid());
				// 7 LATITUDE
				column(dataFile, String(gps.location.lat(), 7));
				// 8 LONGTITUDE
				column(dataFile, String(gps.location.lng(), 7));
				// 9 ALTITUDE 
				column(dataFile, (String)gps.altitude.meters());
				// 10 satelity
				column(dataFile, (String)gps.satellites.value());
				

				//konec řádku
				row(dataFile);



				dataFile.close();				
			} 
			//co když se neotevřel
			else {
				//rozblikám ledku
				blinkSDErr = HIGH;
				Serial.println("SD Write Error!!!");
			}
		}
	}



}

// uložení int do EEPROM
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

void column( File df, String value){
	df.print(value);
	df.print(",");
	Serial.print(value);
	Serial.print(",");
}

void row(File df){
	df.println();
	Serial.println();
}

//String double2string(double n, int ndec) {
//	String r = "";
//	int v = n;
//	r += n;
//	r += '.';
//	int i;
//	for (i=0;i<ndec;i++) {
//		n -= v;
//		n *= 10;
//		v = n;
//		r += v;
//	}
//	return r;
//}

//Rounds down (via intermediary integer conversion truncation)
String double2string(double input,int decimalPlaces){
	if(decimalPlaces!=0){
	String string = String((int)(input*pow(10,decimalPlaces)));
		if(abs(input)<1){
			if(input>0)
				string = "0"+string;
			else if(input<0)
				string = string.substring(0,1)+"0"+string.substring(1);
		}
		return string.substring(0,string.length()-decimalPlaces)+"."+string.substring(string.length()-decimalPlaces);
	}
	else {
		return String((int)input);
	}
}

