static const uint8_t SENSORMODULE_TYPE = 0x01;

struct SMpayload
{
  float sensorA;
  float sensorB;
};
typedef struct SMpayload SMpayload;




static const uint8_t P_SENSOR_INTERRUPT  = 2; // This interrupt says there is an sensor event, it wakes up the sensor module
static const uint8_t P_CM_INTERRUPT  = 9; // This pin is used to wake up the communication module


#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>


SMpayload payload;

static const uint8_t FRAMESTART_BYTE = 0xAA;
volatile bool takeMeasurementInterrupt_flag = false;

void setup() 
{
  Serial.begin(9600);

  pinMode(P_SENSOR_INTERRUPT, INPUT_PULLUP);
  pinMode(P_CM_INTERRUPT, OUTPUT);
  digitalWrite(P_CM_INTERRUPT, HIGH);

  attachInterrupt(0, pin2_isr, LOW); // I USE THIS INTERRUPT (ON PIN2) TO MANUALLY TRIGGER A "MEASUREMENT"

  takeMeasurementInterrupt_flag = false;
}


void loop()
{
  if (takeMeasurementInterrupt_flag == true)
  {
    // Dummy sensor reading
    delay(500);
    payload.sensorA = 2.5;
    payload.sensorB = 2.5;

    // Wake communication module up
    digitalWrite(P_CM_INTERRUPT, LOW);
    delay(5); // Keep signal low for a short while soo the communication module registers the interrupt
    digitalWrite(P_CM_INTERRUPT, HIGH);

    // Wait a short bit until communication module has woken up
    delay(100);
  
    // Pass data to communication module
    uint8_t checksumbyte = 0;
  
    // Framestart byte
    Serial.write(FRAMESTART_BYTE);
    checksumbyte += FRAMESTART_BYTE;
    //Serial.println(FRAMESTART_BYTE,HEX);

    // SM type
    Serial.write((uint8_t) SENSORMODULE_TYPE); // Sensormodule type
    checksumbyte += (uint8_t) SENSORMODULE_TYPE;
    //Serial.println((uint8_t) SENSORMODULE_TYPE,HEX);

    // Length of data
    Serial.write((uint8_t) sizeof(payload)); // Sensormodule type
    checksumbyte += (uint8_t) sizeof(payload);
  
    // Payload struct
    Serial.write((uint8_t*) &payload, sizeof(payload));

    delay(50); // I do not know why this delay is needed, but it does not work without it.
    
    uint8_t* payloadPtr = (uint8_t*) &payload;
    for (uint8_t k=0;k<sizeof(payload);k++)
    {
      checksumbyte += payloadPtr[k];
    }
  
    checksumbyte = 0xFF - checksumbyte; // such that is sums up to 0xFF
    Serial.write(checksumbyte);
    //Serial.println(checksumbyte,HEX);

    takeMeasurementInterrupt_flag = false;
  }


  // This part is necessary such that the communication module will learn that this is an interrupting sensor module
  if (Serial.available())
  {
    while (Serial.available())
    {
      if (Serial.read() == 0xBB)
      {
        Serial.write(0xBB); // CM will read back this byte and recognize that it is an interrupting SM
      }
    }
  }
  
    
  delay(1000);
}


// Pin 2 interrupt
void pin2_isr(void)
{
  //detachInterrupt(0);
  takeMeasurementInterrupt_flag = true;
}
