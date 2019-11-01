#include <PayloadClass.h>
//#include <SensorModuleClass.h>

PayloadClass myMessage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Setup complete");
}

void loop() {
  // Print raw
  Serial.write((uint8_t*) &myMessage.payload, sizeof(myMessage.payload));
  Serial.println("");

  // Print hex empty
  uint8_t* payloadPtr = myMessage.get_payload_ptr();
  for (uint8_t k=0;k<sizeof(myMessage.payload);k++)
  {
    Serial.print(payloadPtr[k], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Print hex populated
  myMessage.set_id(818);
  myMessage.set_SMtype(20);
  myMessage.set_payload(10, 10, 0, 0, 0); 
  for (uint8_t k=0;k<sizeof(myMessage.payload);k++)
  {
    Serial.print(payloadPtr[k], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  
  Serial.println("");
  Serial.println("------------------------------------------------------");
  delay(10000);

}
