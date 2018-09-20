#include <SoftwareSerial.h>

//SoftwareSerial mySerial(12, 11); // RX, TX
SoftwareSerial mySerial(10, 11); // RX, TX
int counter =0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("HardSerial initialized.");
  mySerial.begin(9600);
  mySerial.println("SoftSerial initialized.");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("HardSerial: ");
  Serial.println(counter);
  
  mySerial.print("SoftSerial: ");
  mySerial.println(counter);
  counter++;
  delay(705);
}
