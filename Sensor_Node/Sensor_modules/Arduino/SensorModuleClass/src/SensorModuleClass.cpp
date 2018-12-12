/*
  CommClass
*/


#include "SensorModuleClass.h"
#include "Arduino.h"
#include <stdio.h>
#include <avr/wdt.h> 

SensorModuleClass::SensorModuleClass(){
    payload.payload1= 0;
    payload.payload2= 0;
    payload.payload3= 0;
    payload.payload4= 0;
    payload.payload5= 0;
}
    

void SensorModuleClass::set_sensor_module_type(byte sensor_module_type) {
    sensor_module_type_byte = sensor_module_type;
}

void SensorModuleClass::set_payload(float float1, float float2, float float3, float float4, float float5) {
    payload.payload1= float1;
    payload.payload2= float2;
    payload.payload3= float3;
    payload.payload4= float4;
    payload.payload5= float5;
}

void SensorModuleClass::send_message() {
  uint8_t checksumbyte = 0;
  
  // Framestart byte
  mySerial->write(start_byte);
  checksumbyte += start_byte;
  
  // SM type
  mySerial->write((uint8_t) sensor_module_type_byte); // Sensormodule type
  checksumbyte += (uint8_t) sensor_module_type_byte;
  
  // Length of data
  mySerial->write((uint8_t) sizeof(payload)); // Sensormodule type
  checksumbyte += (uint8_t) sizeof(payload);
  
  // Payload
  mySerial->write((uint8_t*) &payload, sizeof(payload));
  
  uint8_t* payloadPtr = (uint8_t*) &payload;
  for (uint8_t k=0;k<sizeof(payload);k++)
  {
    checksumbyte += payloadPtr[k];
  }
  
  checksumbyte = 0xFF - checksumbyte; // such that is sums up to 0xFF
  mySerial->write(checksumbyte);
}

void SensorModuleClass::set_serial(Stream &serialPort) {
  mySerial = &serialPort;
} 

void SensorModuleClass::set_led_pin(int pin) {
  led_pin = pin;
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
}

void SensorModuleClass::set_debug_pin(int pin) {
  debug_pin = pin;
  pinMode(debug_pin, INPUT);
}

void SensorModuleClass::blink(int n) {
  if (debug_enabled()) {                  // Check debugging switch
    for (int i=0; i<n; i++) {             // Blink LED n times
      digitalWrite(led_pin, HIGH);
      delay(10);
      digitalWrite(led_pin, LOW);
      delay(50);
    }
  }
}


bool SensorModuleClass::debug_enabled(){
  return digitalRead(debug_pin);
}