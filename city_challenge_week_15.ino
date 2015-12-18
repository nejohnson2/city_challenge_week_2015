/*
 * City Challenge Week 2015 - Temp/Hum/AQ
 *
 * Groove Dust Sensor
 * Wire (BLK) - Gnd
 * Wire (RED) - +5v
 * Wire (YLW) - Pin 2
 *
 * SD Card
 * MOSI - Pin 11
 * MISO - Pin 12
 * CLK  - Pin 13
 * CS   - Pin 9
 *
 * HTU21D-F Humidity & Temp Sensor
 * Vin - +5v
 * GND - GND
 * SCL - SCL
 * SDA - SDA
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
#include <Wire.h> //also for Temp/hum sensor
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

// Temp Humidity
#include "Adafruit_HTU21DF.h"
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  dht.begin();
  if (initSD()) {
  }

  Wire.begin();
  
  if (! RTC.isrunning()) {
    Serial.println("Real Time Clock not Running");
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));    
  }
  
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  // For Dust Sensor
  starttime = millis();
}

void loop() {
  DateTime now = RTC.now();
  duration = pulseIn(pin, LOW); //timeout is in micro seconds
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - starttime) > sampletime_ms) //if the sampel time == 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
    lowpulseoccupancy = 0;
    starttime = millis();

    String dataString = ""; // create blank string to write to SD

    dataString += concentration;
    dataString += ",";
    dataString += now.month();
    dataString += "/";
    dataString += now.day();
    dataString += ",";

    dataString += now.hour();
    dataString += ":";
    dataString += now.minute();
    dataString += ":";
    dataString += now.second();
    dataString += ",";

    dataString += htu.readTemperature();
    dataString += ",";
    dataString += htu.readHumidity();
    
    Serial.println(dataString);
    dataFile.println(dataString);
    dataFile.flush();
  }
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
