/************************************
 * Gateway commissioning v1         *
 * Test all LEDs                    *
 ************************************/

int p_LED_r = A0; // Red LED pin
int p_LED_y = A1; // Yellow LED pin
int p_LED_g = A2; // Gren LED pin
int p_pw_sm = 5;  // Power supply sensor module pin
int p_dbg_en = A3;// Debug enable pin

int counter = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(p_LED_r, OUTPUT); // Red LED
  pinMode(p_LED_y, OUTPUT); // Orange LED
  pinMode(p_LED_g, OUTPUT); // Green LED
  pinMode(p_pw_sm, OUTPUT);  // Sensor module supply voltage

  pinMode(p_dbg_en, INPUT); // Debug Enable

  Serial.println("Setup Complete");
}

void loop() {
  digitalWrite(p_LED_r, HIGH);
  delay(200);
  digitalWrite(p_LED_y, HIGH);
  delay(200);
  digitalWrite(p_LED_g, HIGH);
  delay(200);
  digitalWrite(p_pw_sm, HIGH);
  
  
  delay(200);
  digitalWrite(p_LED_r, LOW);
  delay(200);
  digitalWrite(p_LED_y, LOW);
  delay(200);
  digitalWrite(p_LED_g, LOW);
  delay(200);
  digitalWrite(p_pw_sm, LOW);

  delay(200);
  Serial.print(counter++);
  Serial.print(": Debug Enable: ");
  Serial.print(digitalRead(p_dbg_en));
}
