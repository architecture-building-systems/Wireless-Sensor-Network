/*****************************************************************************************************
 * Software Reset v0                                                                                 *
 * Source:                                                                                           *
 *  - http://www.xappsoftware.com/wordpress/2013/06/24/three-ways-to-reset-an-arduino-board-by-code/ *
 *  - http://blog.fsck.com/2014/08/how-to-reboot-an-arduino-leonardo-micro-into-the-bootloader.html  *            *
 *****************************************************************************************************/

#include <avr/wdt.h>            // Needed for SoftwareReset_B()

int counter =5;

void setup() {
  // put your setup code here, to run once:
  //wdt_disable();
  Serial.begin(9600);
  while(!Serial){
    delay(1000);
  }

  Serial.println("Setup Complete");

}

void loop() {
  // put your main code here, to run repeatedly:
  counter--;
  Serial.print("Hello:\t");
  Serial.println(counter);
  delay(1000);
  if (counter==0)
  {
    SoftwareReset_B();
  }

}
void SoftwareReset_B()           // Needs #include <avr/wdt.h> 
{
  Serial.println("Reset!");
  //wdt_reset();                   // does not work on Mega 2560
  wdt_enable(WDTO_30MS);         // WDTO_15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S
  int n = 0;
  while(1)
  {
    Serial.print(n++);
    Serial.println(" Waiting for Reset.");
    delay(1000);
  }
}
