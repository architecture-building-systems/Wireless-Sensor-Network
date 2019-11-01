/*
  CommClass
  This class packs 5 float values in a payload message.
  Author: Mario Frei, mario.frei@arch.ethz.ch
  2018
*/


#include "Arduino.h"
#include <stdio.h>

struct MMpayload
{
  uint16_t mm_id;
  uint8_t  number_of_measurements;
  uint8_t  SMtype;
  float    payload1;
  float    payload2;
  float    payload3;
  float    payload4;
  float    payload5;
};
typedef struct MMpayload MMpayload;

class PayloadClass
{
public:
    MMpayload payload;                   // Payload struct to be sent to main module
    
public:
  PayloadClass();                                                                    // Constructor
  void set_payload(float float1, float float2, float float3, float float4, float float5); // Set floating point numbers payloads and convert floating point numbers to HEX and assign them to the char payload
  void set_id(uint16_t id);                                                               // Set main module id in payloads and convert floating point numbers to HEX and assign them to the char payload
  void set_SMtype(uint8_t type);                                                                 // Calculate checksum and assemble radio message to be transmitted
  uint8_t* get_payload_ptr();
 };
