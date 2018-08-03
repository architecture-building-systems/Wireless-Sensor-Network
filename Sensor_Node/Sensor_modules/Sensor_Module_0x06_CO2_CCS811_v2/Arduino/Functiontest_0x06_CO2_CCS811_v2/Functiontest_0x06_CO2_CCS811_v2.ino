/******************************************************************************

Testcode for C02 & SHT31 Board.

It is possible to check outputs on both serial ports.

******************************************************************************/
#include "SparkFunCCS811.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <Arduino.h>

#define CCS811_ADDR 0x5B //Default I2C Address
//#define CCS811_ADDR 0x5A //Alternate I2C Address

CCS811 mySensor(CCS811_ADDR);
Adafruit_SHT31 sht31 = Adafruit_SHT31();


SoftwareSerial mySerial(7,6); // RX, TX

void setup()
{
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);               // wait 
  digitalWrite(10, LOW);    // turn the LED off by making the voltage LOW
  delay(200);               // wait 
  
  Serial.begin(9600);
  Serial.println("CCS811 & SHT31 Basic Example");
  mySerial.begin(9600);
  mySerial.println("CCS811 & SHT31 Basic Example");

  //It is recommended to check return status on .begin(), but it is not
  //required.
  CCS811Core::status returnCode = mySensor.begin();

  if (! sht31.begin(0x45)) {   // Set to 0x44 for alternate i2c addr
    mySerial.println("Couldn't find SHT31");
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    mySerial.println(".begin() of CCS811 returned with an error.");
    Serial.println(".begin() of CCS811 returned with an error.\n");

    while (1); //Hang if there was a problem.
  } 
}

void loop()
{
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  mySerial.println();
   
 if (! isnan(t)) {  // check if 'is not a number'
    mySerial.print("Temp *C = "); mySerial.println(t);
    Serial.print("Temp *C = "); Serial.println(t);
  } else { 
    mySerial.println("Failed to read temperature");
    Serial.println("Failed to read temperature");
  } 
  
  if (! isnan(h)) {  // check if 'is not a number'
    mySerial.print("Hum. % = "); mySerial.println(h);
    Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    mySerial.println("Failed to read humidity");
    Serial.println("Failed to read humidity");
  }
  mySerial.println();
  Serial.println();
  
  //Check to see if data is ready with .dataAvailable()
  if (mySensor.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    mySensor.readAlgorithmResults();

    mySerial.print("CO2[");
    //Returns calculated CO2 reading
    mySerial.print(mySensor.getCO2());
    mySerial.print("] tVOC[");
    //Returns calculated TVOC reading
    mySerial.print(mySensor.getTVOC());
    mySerial.print("] millis[");
    //Simply the time since program start
    mySerial.print(millis());
    mySerial.print("]");
    mySerial.println();

    Serial.print("CO2[");
    //Returns calculated CO2 reading
    Serial.print(mySensor.getCO2());
    Serial.print("] tVOC[");
    //Returns calculated TVOC reading
    Serial.print(mySensor.getTVOC());
    Serial.print("] millis[");
    //Simply the time since program start
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }

  delay(1000); //Don't spam the I2C bus
}
