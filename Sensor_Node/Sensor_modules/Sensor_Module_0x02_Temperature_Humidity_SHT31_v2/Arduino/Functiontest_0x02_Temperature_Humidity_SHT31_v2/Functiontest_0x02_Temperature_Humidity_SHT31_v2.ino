/******************************************************************************

Testcode for SHT31 Board.

It is possible to check outputs on both serial ports.

******************************************************************************/
 
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <SoftwareSerial.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();
SoftwareSerial mySerial(7,6); // RX, TX

void setup() {
  Serial.begin(9600);
  Serial.println("SHT31 test");
  mySerial.begin(9600);
  mySerial.println("SHT31 test");
  
  if (! sht31.begin(0x45)) {   // Set to 0x44 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    mySerial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
}


void loop() {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);
    mySerial.print("Temp *C = "); mySerial.println(t);
  } else { 
    Serial.println("Failed to read temperature");
    mySerial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
    mySerial.print("Hum. % = "); mySerial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
    mySerial.println("Failed to read humidity");
  }
  Serial.println();
  mySerial.println();
  delay(1000);
}
