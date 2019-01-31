// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7,6); // RX, TX

#define ONE_WIRE_BUS_1 2
#define ONE_WIRE_BUS_2 3

OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);

DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);

/*
 * The setup function. We only start the sensors here
 */
void setup(void)
{
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);               // wait 
  digitalWrite(10, LOW);    // turn the LED off by making the voltage LOW
  delay(200);               // wait 
  
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");
  mySerial.begin(9600);
  mySerial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensor1.begin();
  sensor2.begin();
  sensor1.setResolution(12);
  sensor2.setResolution(12);
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 
  // call sensorX.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  mySerial.print("Requesting temperatures...");
  sensor1.requestTemperatures(); // Send the command to get temperatures
  sensor2.requestTemperatures(); 
  Serial.println("DONE");
  mySerial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 is: ");
  Serial.println(sensor1.getTempCByIndex(0));  
  Serial.print("Temperature for the device 2 is: ");
  Serial.println(sensor2.getTempCByIndex(0)); 
  mySerial.print("Temperature for the device 1 is: ");
  mySerial.println(sensor1.getTempCByIndex(0));  
  mySerial.print("Temperature for the device 2 is: ");
  mySerial.println(sensor2.getTempCByIndex(0)); 

  delay(500);
}
