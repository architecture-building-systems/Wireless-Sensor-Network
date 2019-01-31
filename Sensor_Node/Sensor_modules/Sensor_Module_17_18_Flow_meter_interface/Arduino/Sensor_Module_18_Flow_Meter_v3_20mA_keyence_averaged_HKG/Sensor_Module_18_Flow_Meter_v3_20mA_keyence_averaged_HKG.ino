/****
 * Sensor Module Type 0x12 (=18): Water flow meter
 * This code is meant for a combo sensor module that is used as an analog interface (4-20mA)
 * for the Keyence ultrasonic flowmeter. The sensor module was modified to accept
 * a 4-20mA current loop signal from the flow meter.
 * The resistor_value indicates the value of the resistor which is used to convert the current signal into a voltage.
 * The resistor_value needs to be measured manually and adjusted in the code accordingly.
 * The max_flow indictes the maximum flow rate provided by the flow meter. This is a setting in the flow meter. 
 * 
 * The flow rate is measured every second and added up. For each measurement a counter variable is incremented. Once a measurement
 * data is requested by the main module, the summed up flow rate is divided by the counter variable, effectively yielding an average
 * flow rate between two measurement data request from the main module. The sensor module needs to be powered constantly be the main module.
 * 
 * Author: Mario Frei(2019)
 */

static const uint8_t SENSORMODULE_TYPE = 0x12; // 0x12 [HEX] = 18 [DEC]
static const uint8_t FRAMESTART_BYTE = 0xAA;   // Frame start byte for payload transmission to main module

//******** Manual SETTINGS *********//
float min_flow          = 0;
float max_flow          = 50;                  // [l/min] Maximum flow rate output of flow meter 
//float resistor_value    = 164.4;             // [Ohm] Value of the resistor that converts the current into a voltage 
//float resistor_value    = 163.58;            // [Ohm] Value of the resistor that converts the current into a voltage 
//float resistor_value    = 164.10;            // [Ohm] Value of the resistor that converts the current into a voltage 
//float resistor_value    = 164.85;             // [Ohm] Value of the resistor that converts the current into a voltage  
float resistor_value    = 165.2;             // [Ohm] Value of the resistor that converts the current into a voltage 


int P_input             = A5;                  // Pin number for analog input voltage
int P_request           = A4;                  // Pin number for measurement request signal from main module
int P_LED               = 14;                  // Pin number for debugging LED

float flow_rate_sum     = 0;                   // Temporary variable to sum up all flow rate messages
int counter             = 0;                   // Counts all flow rate measures in order to compute the average = flow_rate_sum/counter
bool request_state      = 0;                   // Stores the state of the request signal from the main module

struct SMpayload                               // Payload struct for easy conversion from float to byte
{
  float volume_flow_rate;
};
typedef struct SMpayload SMpayload;
SMpayload payload;


void setup() {
  Serial.begin(9600);                         // Initialize serial connection
  
  pinMode(P_LED, OUTPUT);                     // Initialize pins
  digitalWrite(P_LED, LOW);
  pinMode(P_input, INPUT);
  pinMode(P_request, INPUT);
  blink();
  
  //Serial.write('Setup erfolgreich');          // Debuggin message (remove for use with main module)
}

void loop()
{
  // START SENSOR MEASUREMENT
  float input_voltage   = analogRead(P_input)*(3.3/1023);       // [V] Read analog input voltage
  float request_voltage = analogRead(P_request)*(3.3/1023);     // [V] Read input voltage of measurement request signal
  request_state = digitalRead(P_request);
  float input_amperage  = (float)(1000.0*input_voltage) / (float)resistor_value; // [mA] Compute amperage based on analog input voltage an apriori measured resistor value
  //float current_flow = map(input_amperage, 4, 20, 0, max_flow); // Convert amperage to mass flow using the conversion factor XXX map uses integer math!!!!
  float current_flow = min_flow + (max_flow-min_flow)/(20.0-4.0)*(input_amperage-4.0);
  flow_rate_sum += current_flow;                                // Add current flow rate to the sum.
  counter++;                                                    // Increase measurement counter. Needed for averaging of the sum
  // END SENSOR MEASUREMENT

/*
  Serial.println("");                                           // Output a lot of debugging messages
  Serial.print("Input Voltage: ");
  Serial.print(input_voltage);
  Serial.println(" V");
  
  Serial.print("Input Amperage: ");
  Serial.print(input_amperage);
  Serial.println(" mA");
  
  Serial.print("Request Voltage: ");
  Serial.print(request_voltage);
  Serial.println(" V");
  
  Serial.print("Request state: ");
  Serial.print(request_state);
  Serial.println("");
  
  Serial.print("Current flow rate: ");
  Serial.print(current_flow);
  Serial.println(" l/min");
  
  Serial.print("Mean flow rate: ");
  Serial.print((float)flow_rate_sum/(float)counter);
  Serial.println(" l/min");
  delay(2000);
/**/

  if (request_state){
    payload.volume_flow_rate = (float)flow_rate_sum/(float)counter;  // Compute mean of flow rate
    flow_rate_sum = 0;                                               // Reset flow rate sum
    counter       = 0;                                               // Reset measurement counter

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
    blink();
  }
  
  delay(1000);
}


/***********************************
 * blink()
 * blinks debugging LED once
 ***********************************/
 
void blink(){
  digitalWrite(P_LED, HIGH);
  delay(10);
  digitalWrite(P_LED, LOW);
}
