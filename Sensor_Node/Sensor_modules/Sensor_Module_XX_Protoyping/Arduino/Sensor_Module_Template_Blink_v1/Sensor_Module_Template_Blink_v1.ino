#include <SoftwareSerial.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
*/

//SoftwareSerial mySerial(8, 9); // RX, TX
int LEDpin = 10;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize LEDpin as an output.
  pinMode(LEDpin, OUTPUT);
  // Serial.begin(9600);
  //Serial.println("Setup Complete");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LEDpin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(400);              // wait for a second
  digitalWrite(LEDpin, LOW);    // turn the LED off by making the voltage LOW
  delay(400);              // wait for a second
  //Serial.println("Hello");
}
