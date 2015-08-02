#include <SoftwareSerial.h>
#include <TinyGPS++.h>


TinyGPSPlus gps;
SoftwareSerial ss(3, 4);

void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  delay(20);
  Serial.println("Reading...");


}

void loop() {
  while (ss.available())
  {
    if (gps.encode(ss.read())) {
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
