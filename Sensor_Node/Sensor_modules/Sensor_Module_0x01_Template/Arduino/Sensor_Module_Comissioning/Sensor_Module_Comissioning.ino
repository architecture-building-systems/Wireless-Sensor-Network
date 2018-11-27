/****
 * For comissioning sensor modules
 *  - Blinks LED
 *  - Outputs message on harware serial port and software serial port
 *  - Mirrors hardware serial port and software serial port
 *  
 *  Author: Mario Frei (2018)
 */

// Pin definitions
int pin_LED = A0;  // Pin for debugging LED
int pin_sRx = 10;  // Pin for receiving software serial
int pin_sTx = 11;  // Pin for transmitting software serial

int counter = 0;   // Counter variable, increases in loop

#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(pin_sRx, pin_sTx); // RX, TX

void setup() {
  pinMode(pin_LED, OUTPUT);               // Set pin mode for LED
  Serial.begin(9600);                     // Start hardware serial
  SoftSerial.begin(9600);                 // Start software serial
  SoftSerial.println("Software Serial."); // "Hello World" for software serial port
  Serial.println("Hardware Serial.");     // "Hello World" for hardware serial port
}

void loop()
{
  // Blink debugging LED
  digitalWrite(pin_LED, LOW);
  delay(100);
  digitalWrite(pin_LED, HIGH);
  delay(100);

  // Output messages on both serial ports
  SoftSerial.println(counter);
  Serial.println(counter++);
  

  // Mirror the serial ports
  while (Serial.available() > 0) {
    char inByte = Serial.read();
    SoftSerial.write(inByte);
  }
    while (SoftSerial.available() > 0) {
    char inByte = SoftSerial.read();
    Serial.write(inByte);
  }
}
