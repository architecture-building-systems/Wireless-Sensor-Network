/****
 * Sensor Module Type 0x12 (=18): Water flow meter
 * This code is meant for a combo sensor module that is used as an analog interface
 * for the Siemens Sitrans ultrasonic flowmeter. The sensor module was modified to accept
 * a 0-10V analog voltage signal from the flow meter.
 * The conversion_factor_voltage indicates the ratio of the voltage divider and needs to be measured manually.
 * The max_flow indictes the maximum flow rate provided by the flow meter. This is a setting in the flow meter.
 */                                      

static const uint8_t SENSORMODULE_TYPE = 0x12;   // 0x12 [HEX] = 18 [DEC]

static const uint8_t P_ANALOG_INPUT        = A5; // This pin is attached to the output of the debouncer IC
static const uint8_t P_MEASUREMENT_REQUEST = A4; // High signal from communication board for requesting a measurement
static const uint8_t P_LED                 = A0; // Debug LED pin

float conversion_factor_voltage = 3.13;          // Conversion factor to compensate voltage devider. Needs to measured [-]
float min_flow = 0;                              // Min flow rate output of flow meter[(l/)/V]
float max_flow = 20;                             // Max flow rate output of flow meter[(l/)/V]

float input_voltage_low      = 0;                // Input voltage after voltage devider 0-3.3V
float input_voltage_high     = 0;                // Voltage from flow meter 0-10V
float flow_rate_current      = 0;                // Instantaneous mass flow rate
float flow_rate_sum          = 0;                // Sum of mass flow rates
int counter                  = 0;                // Counter variable, gets increased with each measurement. Needed to compute average = sum/counter

struct SMpayload                                 // Payload struct for easy conversion from float to byte
{
  float mass_flow_rate;
};
typedef struct SMpayload SMpayload;

SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;

void setup() {

  Serial.begin(9600);                           // Setup serial connection
//  Serial.println("Setup begins");
  
  pinMode(P_MEASUREMENT_REQUEST, INPUT);        // Setup pins
  pinMode(P_ANALOG_INPUT, INPUT);
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, LOW);
}

void loop()
{

  input_voltage_low      = analogRead(P_ANALOG_INPUT)*3.3/1023;           // Read analog input voltage
  input_voltage_high     = input_voltage_low * conversion_factor_voltage; // Convert input voltage to flow meter output voltage
  flow_rate_current      = map(input_voltage_high, 0, 10, 0, max_flow);   // Convert flow meter output voltage to flow rate   XXX map uses integer math!
  flow_rate_current      = min_flow + (max_flow-min_flow)/(10-0)*(input_voltage_high-0.0);   // Convert flow meter output voltage to flow rate
  flow_rate_sum         += flow_rate_current;                             // Sum up mass flow rates
  counter++;                                                              // Increment counter variable
  // END SENSOR MEASUREMENT
  /*
  Serial.println("");                                                     // Output a lot of debugging messages
  Serial.print("Input Voltage low: ");
  Serial.print(input_voltage_low);
  Serial.println(" V");
  
  Serial.print("Input Voltage high: ");
  Serial.print(input_voltage_high);
  Serial.println(" V");

  Serial.print("Current flow rate: ");
  Serial.print(flow_rate_current);
  Serial.println(" l/min");
  
  Serial.print("Mean flow rate: ");
  Serial.print((float)flow_rate_sum/(float)counter);
  Serial.println(" l/min");
  delay(2000);
/**/
  
  if (digitalRead(P_MEASUREMENT_REQUEST)==HIGH){

    payload.mass_flow_rate = (float)flow_rate_sum/(float)counter;    // Compute mean of flow rate
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

    // Indicate end of transmission by blinking LED once
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
