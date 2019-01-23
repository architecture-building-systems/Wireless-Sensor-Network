/****
 * Sensor Module Type 0x13 (=19): Water flow meter
 * This code is meant for a combo sensor module that is used as an analog interface
 * for the Siemens Sitrans ultrasonic flowmeter. The sensor module was modified to accept
 * a 4-20mA current loop signal from the flow meter.
 * The resistor_value indicates the value of the resistor which is used to convert the current signal into a voltage.
 * The resistor_value needs to be measured manually.
 * The max_flow indictes the maximum flow rate provided by the flow meter. This is a setting in the flow meter.   
 * Author: Mario Frei(2018)
 */

static const uint8_t SENSORMODULE_TYPE = 0x11; // 0x11 [HEX] = 17 [DEC]
static const uint8_t FRAMESTART_BYTE = 0xAA;   // Frame start byte for payload transmission to main module

int P_input             = A5;                  // Pin number for analog input voltage
int P_request           = A4;                  // Pin number for measurement request signal from main module
int P_LED               = 14;                  // Pin number for debugging LED

float flow_rate_sum     = 0;                   // Temporary variable to sum up all flow rate messages
int counter             = 0;                   // Counts all flow rate measures in order to compute the average = flow_rate_sum/counter
bool request_state      = 0;                   // Stores the state of the request signal from the main module

bool digitalInput_old = 0;                     // State of input pin from previous loop
bool digitalInput_new = 0;                     // State of input pin from current loop
float flow_volume = 0;                         // Summed up flow volume
float flow_rate = 0;                           // Flow rate = flow volume / timestamp difference
float integration_unit = 1;                    // Integration unit (setting in flow meter) [l]
float timestamp_old = 0;                       // Timestamp for debugging
bool LED_state = 0;

struct SMpayload                               // Payload struct for easy conversion from float to byte
{
  float flow_volume;
};
typedef struct SMpayload SMpayload;
SMpayload payload;


void setup() {
  Serial.begin(9600);                         // Initialize serial connection
  
  pinMode(P_LED, OUTPUT);                     // Initialize pins
  digitalWrite(P_LED, LED_state);
  pinMode(P_input, INPUT);
  pinMode(P_request, INPUT);
  blink(3);
  
  //Serial.write('Setup erfolgreich');          // Debuggin message (remove for use with main module)
}

void loop()
{
  // START SENSOR MEASUREMENT
  request_state = digitalRead(P_request);
  digitalInput_new = digitalRead(P_input);
  // Blink debugging LED
  if (digitalInput_new) {
    LED_state = 1;
  } else {
    LED_state = 0;
  }
  digitalWrite(P_LED, LED_state);
  
  if (digitalInput_new != digitalInput_old){
    flow_volume += integration_unit*0.5;
  }
  
  counter++;                                                    // Increase measurement counter. Needed for averaging of the sum
  // END SENSOR MEASUREMENT

/**
  if (millis()-timestamp_old>1500){
    Serial.println("");                                         // Output a lot of debugging messages
    Serial.print("Input State: ");
    Serial.print(digitalInput_new);
    Serial.println("");
    
    Serial.print("Flow_volume: ");
    Serial.print(flow_volume);
    Serial.println(" l");
    
    Serial.print("Request state: ");
    Serial.print(request_state);
    Serial.println("");
    
    timestamp_old = millis();
  }
/**/

  if (request_state){
    payload.flow_volume = flow_volume;
    flow_volume = 0;                                               // Reset flow rate sum
    
    // Transmit payload message to main module
    uint8_t checksumbyte = 0;
  
    // Framestart byte
    Serial.write(FRAMESTART_BYTE);
    checksumbyte += FRAMESTART_BYTE;
  
    // SM type
    Serial.write((uint8_t) SENSORMODULE_TYPE); // Sensormodule type
    checksumbyte += (uint8_t) SENSORMODULE_TYPE;
  
    // Length of data
    Serial.write((uint8_t) sizeof(payload)); // Sensormodule payload
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
    blink(2);
  }
  digitalInput_old = digitalInput_new;
  delay(200);
}


/***********************************
 * blink()
 * blinks debugging LED once
 ***********************************/
 
void blink(int n){
  for (int i=0; i<n; i++){
    digitalWrite(P_LED, !LED_state);
    delay(10);
    digitalWrite(P_LED, LED_state);
  }
}
