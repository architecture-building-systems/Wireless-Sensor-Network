/****
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 */

static const uint8_t SENSORMODULE_TYPE = 0x05;
static const uint8_t P_DEBOUNCER_INPUT   = 4;  // This pin is attached to the output of the debouncer IC
static const uint8_t P_LED               = A0; // Debug LED pin


// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float opening_duration;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;
float opening_start_time = 0;
float opening_end_time = 0;

void setup() {
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, HIGH);
  Serial.begin(9600);
  delay(200);
  // This part is necessary such that the communication module will learn that this is an interrupting sensor module
  if (Serial.available())
  {
    while (Serial.available())
    {
      if (Serial.read() == 0xBB)
      {
        Serial.write(0xBB); // CM will read back this byte and recognize that it is an interrupting SM
        return;
      }
    }
  }

  pinMode(P_DEBOUNCER_INPUT, INPUT);

  delay(1000);
  opening_start_time = millis();

  //Serial.println("\nSetup complete");
  // START SENSOR MEASUREMENT
  while ( digitalRead(P_DEBOUNCER_INPUT)==HIGH){  // Wait while windows is open
    //Serial.println("waiting for windows to close");
    digitalWrite(P_LED, HIGH);
    delay(100);
    digitalWrite(P_LED, LOW);
    delay(500);  
  }
  
  digitalWrite(P_LED, LOW);
  opening_end_time = millis();
  
  payload.opening_duration = (opening_end_time-opening_start_time)/1000;
  //Serial.println(payload.opening_duration);
  
  
  // END SENSOR MEASUREMENT
  sent2CM();
}

void loop()
{
  delay(10000);
}

void sent2CM(){
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
