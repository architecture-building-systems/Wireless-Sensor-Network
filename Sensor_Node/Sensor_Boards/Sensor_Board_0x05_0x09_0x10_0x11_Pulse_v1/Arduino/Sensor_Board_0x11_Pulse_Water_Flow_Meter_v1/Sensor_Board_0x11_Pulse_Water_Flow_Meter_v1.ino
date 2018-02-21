/****
 * For creating a sensormodule with a new sensor:
 * Only change:
 * - struct SMpayload to contain the appropriate variables
 * - in setup(), between // START SENSOR MEASUREMENT and // END SENSOR MEASUREMENT add code that gets the measurement and fills the payload's variables with the data.
 */

static const uint8_t SENSORMODULE_TYPE = 0x0A; // 0A [HEX] = 10 [DEC]

static const uint8_t P_DEBOUNCER_OUTPUT    = 4;  // This pin is attached to the output of the debouncer IC
static const uint8_t P_MEASUREMENT_REQUEST = A5; // High signal from communication board
static const uint8_t P_INTERRUPT           = 2;  // This pin is also attached to the output of the debouncer IC and this pin is capable of interrupts
static const uint8_t P_LED                 = A0; // Debug LED pin

volatile unsigned int pulse_count = 0;
static const int meter_constant = 1; // imp/l   (1 imp/l = 1 l/imp)
unsigned long timestamp = 0;


// Temperature reading is read as float. Use 1 byte for value before comma and one byte for value after comma
struct SMpayload
{
  float liter;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {
  Serial.begin(9600);
  //Serial.println("Setup begins");
  // START SENSOR MEASUREMENT
  pinMode(P_MEASUREMENT_REQUEST, INPUT);
  pinMode(P_DEBOUNCER_OUTPUT, INPUT);
  pinMode(P_INTERRUPT, INPUT);
  attachInterrupt(digitalPinToInterrupt(P_INTERRUPT), interruptAction, FALLING);
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, LOW);
}

void loop()
{
  //Serial.print("Hello ");
  //Serial.println(pulse_count);
  //delay(1000);
    
  if (digitalRead(P_MEASUREMENT_REQUEST)==HIGH){
    //digitalWrite(P_LED, HIGH);
    //Serial.println("Measurement Request");
    payload.liter = (float)pulse_count/(float)meter_constant;
    pulse_count = 0;
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
    delay(1000);
  }
}

void interruptAction(){
    if ((millis()-timestamp)>1000)
    {
      pulse_count++;
      timestamp = millis();
      digitalWrite(P_LED, HIGH);
      pause();
      digitalWrite(P_LED, LOW);
    }

}

void pause(){
  for (float i=0; i<1500; i++){
    float a = i/4.0;  
  }
}
