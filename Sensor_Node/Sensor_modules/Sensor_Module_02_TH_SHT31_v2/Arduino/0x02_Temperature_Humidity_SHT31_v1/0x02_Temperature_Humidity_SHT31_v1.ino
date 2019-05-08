/*********************************************************
* Sensor module 0x02 temperature & humidity (SHT31)
* 
* This script is reads the SHT31 sensor and writes
* the data via SoftwareSerial to the main module.
* 
* Author: Mario Frei
* 2018
**********************************************************/

#include <SensorModuleClass.h>
#include "Adafruit_SHT31.h"
#include <SoftwareSerial.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();

byte SensorModuleType = 0x02;
int P_LED             = 10;                                // Assign pin number of debugging LED
int P_switch          = A3;                                // Assign pin number of debugging switch
SoftwareSerial SoftSerial(7, 6);                           // Initilize SoftwareSerial (RX, TX)
SensorModuleClass SensorModule;                            // Initilize sensor module class

void setup() {
  // Initilize stuff
  sht31.begin(0x45);                                        // Initilize sensors (alternate i2c addr: 0x44)

  SoftSerial.begin(9600);                                   // Initialize software serial connection
  SensorModule.set_sensor_module_type(SensorModuleType);    // Assign sensor module type
  SensorModule.set_led_pin(P_LED);                          // Assign pin of debugging LED
  SensorModule.set_debug_pin(P_switch);                     // Assign pin of debugging switch
  SensorModule.set_serial(SoftSerial);                      // Handover serial connection
  
  // Read sensors
  float temperature = sht31.readTemperature();              // Read temperature from SHT31 sensor
  float humidity = sht31.readHumidity();                    // Read humidity from SHT31 sensor
  // Send data
  SensorModule.blink(1);                                    // Blink debuggin LED once    
  SensorModule.set_payload(temperature, humidity, 0, 0, 0); // Assign measured data to payload.
  SensorModule.send_message();                              // Compute checksum and transmit message via serial bus   
}

void loop(){}                                               // Do nothing and wait to be turned of by the main module
