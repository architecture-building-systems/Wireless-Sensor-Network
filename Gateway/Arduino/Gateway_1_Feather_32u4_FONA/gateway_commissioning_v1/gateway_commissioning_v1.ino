/************************************
 * Gateway commissioning v1         *
 * Test all LEDs                    *
 ************************************/



int counter = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A0, OUTPUT); // For communication module commissioning

  Serial.println("Setup Complete");
}

void loop() {
  digitalWrite(A3, HIGH);
  delay(200);
  digitalWrite(A1, HIGH);
  delay(200);
  digitalWrite(A2, HIGH);
  digitalWrite(A0, HIGH); // For communication module commissioning
  delay(200);
  digitalWrite(A3, LOW);
  delay(200);
  digitalWrite(A1, LOW);
  delay(200);
  digitalWrite(A2, LOW);
  digitalWrite(A0, LOW); // For communication module commissioning
  delay(200);
  Serial.println(counter++);
}
