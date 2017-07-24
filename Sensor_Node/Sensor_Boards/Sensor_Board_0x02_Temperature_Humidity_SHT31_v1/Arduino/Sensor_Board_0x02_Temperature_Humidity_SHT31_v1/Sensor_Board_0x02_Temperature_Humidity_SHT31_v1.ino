/****
 * SHT31
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 * Source: SHT31 Library example
 */



 
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

static const uint8_t SENSORMODULE_TYPE = 0x02;
int P_LED = 14;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float temperature;
  float humidity;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {
  Serial.begin(9600);
  Serial.write('Setup erfolgreich');
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, LOW);
  sht31.begin(0x44);// Set to 0x45 for alternate i2c addr
  blink();
}

void loop()
{
  // START SENSOR MEASUREMENT
  
  // Dummy sensor 
  //delay(500);
  payload.temperature = sht31.readTemperature();
  payload.humidity = sht31.readHumidity();
  
  // END SENSOR MEASUREMENT

  uint8_t checksumbyte = 0;

  // Framestart byte
  Serial.write(FRAMESTART_BYTE);
  checksumbyte += FRAMESTART_BYTE;

  // SM type
  Serial.write((uint8_t) SENSORMODULE_TYPE); // Sensormodule type
  checksumbyte += (uint8_t) SENSORMODULE_TYPE;

  // Length of data
  Serial.write((uint8_t) sizeof(payload)); // Sensormodule type
  checksumbyte += (uint8_t) sizeof(payload);

  // Payload
  Serial.write((uint8_t*) &payload, sizeof(payload));
  
  uint8_t* payloadPtr = (uint8_t*) &payload;
  for (uint8_t k=0;k<sizeof(payload);k++)
  {
    checksumbyte += payloadPtr[k];
  }

  checksumbyte = 0xFF - checksumbyte; // such that is sums up to 0xFF
  Serial.write(checksumbyte);
  blink();
  delay(1000);
}

void blink(){
  digitalWrite(P_LED, HIGH);
  delay(10);
  digitalWrite(P_LED, LOW);
}
