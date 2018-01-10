/****
 * CO2meter S8
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 * Source: K30 Library example
 */

static const uint8_t SENSORMODULE_TYPE = 0x06;
// CO2 Sensor libraries:
#include "SoftwareSerial.h"
// SHT31 libraries:
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

SoftwareSerial K_30_Serial(8,9); //RX TX (connect an jeweils TX RX von Device)
Adafruit_SHT31 sht31 = Adafruit_SHT31();

byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};  //Command packet to read Co2 (see app note)
byte response[] = {0,0,0,0,0,0,0};  //create an array to store the response

int valMultiplier = 1;//multiplier for value. default is 1. set to 3 for K-30 3% and 10 for K-33 ICB

// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float co2;
//  float temperature;
//  float humidity;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  Serial.begin(9600);
  // START SENSOR MEASUREMENT
  K_30_Serial.begin(9600); 
  sendRequest(readCO2);
  unsigned long valCO2 = getValue(response);
  payload.co2 = valCO2;
  
  //sht31.begin(0x44);// Set to 0x45 for alternate i2c addr
  //payload.temperature = sht31.readTemperature();
  //payload.humidity = sht31.readHumidity();
  
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

  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  digitalWrite(14, HIGH);
  delay(100);
  digitalWrite(14, LOW);
}

void loop()
{
  delay(10000);
}

void sendRequest(byte packet[])
{
  while(!K_30_Serial.available())  //keep sending request until we start to get a response
  {
    K_30_Serial.write(readCO2,7);
    delay(50);
  }
  
  int timeout=0;  //set a timeoute counter
  while(K_30_Serial.available() < 7 ) //Wait to get a 7 byte response
  {
    timeout++;  
    if(timeout > 10)    //if it takes to long there was probably an error
      {
        while(K_30_Serial.available())  //flush whatever we have
          K_30_Serial.read();
          
          break;                        //exit and try again
      }
      delay(50);
  }
  
  for (int i=0; i < 7; i++)
  {
    response[i] = K_30_Serial.read();
  }  
}

unsigned long getValue(byte packet[])
{
    int high = packet[3];                        //high byte for value is 4th byte in packet in the packet
    int low = packet[4];                         //low byte for value is 5th byte in the packet

  
    unsigned long val = high*256 + low;                //Combine high byte and low byte with this formula to get value
    return val* valMultiplier;
}

