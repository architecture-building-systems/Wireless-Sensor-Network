/****************************************************************
 * Gateway 2 Mega - LED test
 */

int LED_pin = 3;
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_pin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Setup complete.");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_pin, HIGH);
  delay(300);
  digitalWrite(LED_pin, LOW);
  delay(300);
  Serial.println(counter++);
}
