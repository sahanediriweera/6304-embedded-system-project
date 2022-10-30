#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int RXPin = 3, TXPin = 4;
const uint32_t GPSBaud = 9600; //Default baud of NEO-6M is 9600


TinyGPSPlus gps; // the TinyGPS++ object
SoftwareSerial gpsSerial(RXPin, TXPin); // the serial interface to the GPS device

bool initialized_coordinates = 0;
double prelong = -1.0;
double prelatt = -1.0;
double nowlong = -1.0;
double nowlatt = -1.0;
double total_distance = 0.0;

#include <LiquidCrystal.h>

LiquidCrystal lcd(1,2,5,6,7,8);
//LiquidCrystal lcd(13,8,9,10,11,12);
/*
13-8
12-7
11-6
10-5
9-2
8-1
*/

/*
K - Ground
A - 5V
D7 - 12
D6 - 11
D5 - 10
D4 - 9
E  - 8
RW - Ground
RS - 7
VO - POT
VDD - 5V
VSS - Ground 
*/
int x= 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  Serial.println(F("Arduino - GPS module"));

  lcd.begin(16, 2);
   
  lcd.print("Hello, World!");
}

void loop() {
  if (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        Serial.print(F("- latitude: "));
        Serial.println(gps.location.lat());

        Serial.print(F("- longitude: "));
        Serial.println(gps.location.lng());
                
        if(!initialized_coordinates){
          if (prelong == -1.0 && prelatt == -1.0 )
          {
            prelong = gps.location.lng();
            prelatt = gps.location.lat();
            initialized_coordinates = true;
            delay(1000);
          }
        }
        nowlong = gps.location.lng();
        nowlatt = gps.location.lat();
        float flat1 = prelatt;
        float flon1 = prelong;
        float dist_calc = 0;
        float dist_calc2 = 0;
        float dist_calcKM = 0;
        float diflat = 0;
        float diflon = 0;
        float x2lat = nowlatt;
        float x2lon = nowlong;
  
        diflat = radians(x2lat - flat1);
        flat1 = radians(flat1);
        x2lat = radians(x2lat);
        diflon = radians((x2lon) - (flon1));
        dist_calc = (sin(diflat / 2.0) * sin(diflat / 2.0));
        dist_calc2 = cos(flat1);
        dist_calc2 *= cos(x2lat);
        dist_calc2 *= sin(diflon / 2.0);
        dist_calc2 *= sin(diflon / 2.0);
        dist_calc += dist_calc2;
        dist_calc = (2 * atan2(sqrt(dist_calc), sqrt(1.0 - dist_calc)));
        dist_calc *= 6371000.0;
        dist_calcKM = dist_calc / 1000;
        if(dist_calc>10.0){
        total_distance += dist_calc;
        prelong = nowlong;
        prelatt = nowlatt;
        }
        Serial.print("Total Distance : ");
        Serial.print(total_distance);
        Serial.println();
        lcd.setCursor(0, 1);
  // print the number of seconds since reset:

        lcd.print(millis() / 1000);

        lcd.setCursor(0,0);

        lcd.print(x++);
        Serial.print(F("- altitude: "));
        if (gps.altitude.isValid())
          Serial.println(gps.altitude.meters());
        else
          Serial.println(F("INVALID"));
      } else {
        Serial.println(F("- location: INVALID"));
      }

      Serial.print(F("- speed: "));
      if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph());
        Serial.println(F(" km/h"));
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.print(F("- GPS date&time: "));
      if (gps.date.isValid() && gps.time.isValid()) {
        Serial.print(gps.date.year());
        Serial.print(F("-"));
        Serial.print(gps.date.month());
        Serial.print(F("-"));
        Serial.print(gps.date.day());
        Serial.print(F(" "));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        Serial.println(gps.time.second());
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.println();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}