/******************************************************************************
  BasicReadings.ino

  Marshall Taylor @ SparkFun Electronics
  Nathan Seidle @ SparkFun Electronics

  April 4, 2017

  https://github.com/sparkfun/CCS811_Air_Quality_Breakout
  https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library

  Read the TVOC and CO2 values from the SparkFun CSS811 breakout board

  This is the simplest example.  It throws away most error information and
  runs at the default 1 sample per second.

  A new sensor requires at 48-burn in. Once burned in a sensor requires
  20 minutes of run in before readings are considered good.

  Hardware Connections (Breakoutboard to Arduino):
  3.3V to 3.3V pin
  GND to GND pin
  SDA to A4
  SCL to A5

  Resources:
  Uses Wire.h for i2c operation

  Development environment specifics:
  Arduino IDE 1.8.1

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).

  Please review the LICENSE.md file included with this example. If you have any questions
  or concerns with licensing, please contact techsupport@sparkfun.com.

  Distributed as-is; no warranty is given.
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
  mySerial.begin(9600);
  mySerial.println("CCS811 & SHT31 Basic Example");

  //It is recommended to check return status on .begin(), but it is not
  //required.
  CCS811Core::status returnCode = mySensor.begin();
  
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    mySerial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    mySerial.println(".begin() of CCS811 returned with an error.");
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
  } else { 
    mySerial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    mySerial.print("Hum. % = "); mySerial.println(h);
  } else { 
    mySerial.println("Failed to read humidity");
  }
  mySerial.println();
  
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
  }

  delay(1000); //Don't spam the I2C bus
}
