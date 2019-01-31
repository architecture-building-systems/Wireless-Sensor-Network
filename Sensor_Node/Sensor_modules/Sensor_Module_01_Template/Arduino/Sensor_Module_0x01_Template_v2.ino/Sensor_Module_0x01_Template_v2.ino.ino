/**************************************************************************
* Sensor module 0x01 test
* 
* This script writes dummy values to the main module via software serial.
* 
* Author: Mario Frei
* 2018
***************************************************************************/

#include <SensorModuleClass.h>
#include <SoftwareSerial.h>

byte SensorModuleType = 0x01;
int P_LED             = 10;                                // Assign pin number of debugging LED
int P_switch          = A3;                                // Assign pin number of debugging switch
SoftwareSerial SoftSerial(7, 6);                           // Initilize SoftwareSerial (RX, TX)
SensorModuleClass SensorModule;                            // Initilize sensor module class

void setup() {
  // Initilize stuff
  SoftSerial.begin(9600);                                   // Initialize software serial connection
  SensorModule.set_sensor_module_type(SensorModuleType);    // Assign sensor module type
  SensorModule.set_led_pin(P_LED);                          // Assign pin of debugging LED
  SensorModule.set_debug_pin(P_switch);                     // Assign pin of debugging switch
  SensorModule.set_serial(SoftSerial);                      // Handover serial connection
  
  // "Read sensors"
  float value1 = 1.01;                                      // Assign dummy values. Replace this with readings from sensors
  float value2 = 2.02;                                      // 
  float value3 = 3.03;                                      // 
  
  // Send data
  SensorModule.blink(1);                                    // Blink debuggin LED once    
  SensorModule.set_payload(value1, value2, value3, 0, 0);   // Assign measured data to payload. (Maximum 5 values)
  SensorModule.send_message();                              // Compute checksum and transmit message via serial bus   
}

void loop(){}                                               // Do nothing and wait to be turned of by the main module
