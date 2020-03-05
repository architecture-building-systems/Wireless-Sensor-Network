/****************************************************************
 * Gateway 2 Mega - Serial test
 */

int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Setup complete.");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(counter++);
  delay(750);
}
