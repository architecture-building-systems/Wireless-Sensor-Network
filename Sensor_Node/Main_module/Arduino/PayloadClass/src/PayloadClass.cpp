/*
  CommClass
*/


#include "PayloadClass.h"
#include "Arduino.h"
#include <stdio.h>
#include <avr/wdt.h> 

PayloadClass::PayloadClass(){
    payload.mm_id    = 0;
    payload.number_of_measurements = 1;
    payload.SMtype   = 0;
    payload.payload1 = 0;
    payload.payload2 = 0;
    payload.payload3 = 0;
    payload.payload4 = 0;
    payload.payload5 = 0;
}

void PayloadClass::set_payload(float float1, float float2, float float3, float float4, float float5) {
    payload.payload1= float1;
    payload.payload2= float2;
    payload.payload3= float3;
    payload.payload4= float4;
    payload.payload5= float5;
}

void PayloadClass::set_id(uint16_t id) {
    payload.mm_id = id;
}

void PayloadClass::set_SMtype(uint8_t type) {
    payload.SMtype = type;
}

uint8_t* PayloadClass::get_payload_ptr() {
    uint8_t* payloadPtr = (uint8_t*) &payload;
    return payloadPtr;
}