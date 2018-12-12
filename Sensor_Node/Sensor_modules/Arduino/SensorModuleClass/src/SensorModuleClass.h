/*
  CommClass
  This class packs 5 float values in a payload message.
  Author: Mario Frei, mario.frei@arch.ethz.ch
  2018
*/


#include "Arduino.h"
#include <stdio.h>

struct SMpayload
{
  float payload1;
  float payload2;
  float payload3;
  float payload4;
  float payload5;
};
typedef struct SMpayload SMpayload;

class SensorModuleClass
{
public:
    byte start_byte              = 0xaa; // Start byte
    byte sensor_module_type_byte = 0x00; // Type of sensor module (e.g. temperature and humidity measurement)
    SMpayload payload;                   // Payload struct to be sent to main module
    int led_pin;                         // Pin number of debugging LED
    int debug_pin;                       // Pin number of debugging switch
    Stream *mySerial;                    // Pointer to serial object
 
public:
  SensorModuleClass();                                                                    // Constructor
  void set_sensor_module_type(byte sensor_module_type);                                   // Set type of message
  void set_payload(float float1, float float2, float float3, float float4, float float5); // Set floating point numbers payloads and convert floating point numbers to HEX and assign them to the char payload
  void send_message();                                                                    // Calculate checksum and assemble radio message to be transmitted
  void set_led_pin(int pin);                                                              // Assign pin of debugging LED
  void set_debug_pin(int pin);                                                            // Assign pin of debugging switch
  void blink(int n);                                                                      // Blink LED n times
  void set_serial(Stream &serialPort);                                                    // Assigning serial object
  bool debug_enabled();                                                                   // Check debugging switch
};
