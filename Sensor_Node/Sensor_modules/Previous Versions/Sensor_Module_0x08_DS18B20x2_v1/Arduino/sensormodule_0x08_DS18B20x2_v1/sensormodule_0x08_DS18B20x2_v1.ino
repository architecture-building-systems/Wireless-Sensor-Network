/****
 * DS18B20x2
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 */

static const uint8_t SENSORMODULE_TYPE = 0x08;

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_1 2
#define ONE_WIRE_BUS_2 3

OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);

DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);

// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float temp1;
  float temp2;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {
  Serial.begin(9600);
  blink();

  // START SENSOR MEASUREMENT
    sensor1.begin();
    sensor2.begin();
    sensor1.setResolution(12);
    sensor2.setResolution(12);
    sensor1.requestTemperatures();
    sensor2.requestTemperatures();

  
  // Dummy sensor 
  delay(500);
  payload.temp1 = sensor1.getTempCByIndex(0);
  payload.temp2 = sensor2.getTempCByIndex(0);
  
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
}

void loop()
{
  delay(10000);
}

void blink(){
  int P_LED = 14;
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, HIGH);
  delay(10);
  digitalWrite(P_LED, LOW);
}
