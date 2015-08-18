/*
 * City Challenge Week 2015
 *
 * DHT 22
 * Wire (BLK) - Gnd
 * Wire (RED) - +5v
 * Wire (YLW) - Pin 3
 *
 * Groove Dust Sensor
 * Wire (BLK) - Gnd
 * Wire (RED) - +5v
 * Wire (YLW) - Pin 2
 *
 * MEMS Microphone (3.3v)
 * Anolog Read - A0
 *
 * SD Card
 * MOSI - Pin 11
 * MISO - Pin 12
 * CLK  - Pin 13
 * CS   - Pin 9
 *
 */

// SD Card Libraries
#include <SPI.h>
#include <SD.h>
const int chipSelect = 9;
File dataFile;

// DHT TEMP SENSOR
#include <DHT.h>
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Real Time Clock
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

// Groove Dust Sensor
int pin = 2;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  dht.begin();
  if (initSD()) {
  }

  Wire.begin();
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (! RTC.isrunning()) {
    Serial.println("Real Time Clock not Running");
  }

  // For Dust Sensor
  starttime = millis();
}

void loop() {
  DateTime now = RTC.now();
  //duration = pulseIn(pin, LOW, 250000); //timeout is in micro seconds
  lowpulseoccupancy = lowpulseoccupancy + duration;
  
  if ((millis() - starttime) > sampletime_ms) //if the sampel time == 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  // put your main code here, to run repeatedly:
  float audio = analogRead(A0);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  String dataString = ""; // create blank string to write to SD

//  dataString += concentration;
//  dataString += ",";
//  dataString += now.month();
//  dataString += "/";
//  dataString += now.day();
//  dataString += ",";

  dataString += now.hour();
  dataString += ":";
  dataString += now.minute();
  dataString += ":";
  dataString += now.second();
  dataString += ",";

//  dataString += h;
//  dataString += ",";  
//  dataString += t;
//  dataString += ",";
  dataString += audio;

  //Serial.println(dataString);
  dataFile.println(dataString);
  dataFile.flush();
  
}

boolean initSD() {
  // this is the default slave select.  must be set to output
  pinMode(SS, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    // don't do anything more:
    while (1) ;
  }
  // Open up the file we're going to log to!
  dataFile = SD.open("datafile.txt", FILE_WRITE);
  if (!dataFile) {
    // Wait forever since we cant write data
    while (1) ;
  }
  return true;
}
