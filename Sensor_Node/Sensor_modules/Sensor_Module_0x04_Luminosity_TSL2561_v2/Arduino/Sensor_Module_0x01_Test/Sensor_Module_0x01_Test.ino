/****
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 */
#include <EEPROM.h>
byte EEPROM_content0 = 0;
byte EEPROM_content1 = 0;
int debug_LED = A0;

static const uint8_t SENSORMODULE_TYPE = 0x01;

// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float sensorA;
  float sensorB;
  float sensorC;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {
  Serial.begin(9600);
  pinMode(debug_LED, OUTPUT);
  digitalWrite(debug_LED, HIGH);

  // START SENSOR MEASUREMENT

  // for more interesting output
  EEPROM_content0 = EEPROM.read(0);
  EEPROM_content1 = EEPROM.read(1);

  if (EEPROM_content0<5 || EEPROM_content0>10){ // Initialize values
    EEPROM_content0 = 5;
    EEPROM_content1 = 1;
    EEPROM.write(0, EEPROM_content0);
    EEPROM.write(1, EEPROM_content1);
  }
  
  if (EEPROM_content0 >=10){
    EEPROM_content1 = 0;
    EEPROM.write(1, EEPROM_content1);
  }
  if (EEPROM_content0 <= 5){
    EEPROM_content1 = 1;
    EEPROM.write(1, EEPROM_content1);
  }


  
  if (EEPROM_content1 == 0){
    EEPROM.write(0, EEPROM_content0-1);
  }
  if (EEPROM_content1 == 1){
    EEPROM.write(0, EEPROM_content0+1);
  }

  // Dummy sensor 
  delay(500);
  payload.sensorA = EEPROM_content0;
  //payload.sensorA = 1;
  payload.sensorB = 2;
  payload.sensorC = 3;  
  
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
}

void loop()
{
  digitalWrite(debug_LED, LOW);
  delay(100);
  digitalWrite(debug_LED, HIGH);
  delay(100);
}
