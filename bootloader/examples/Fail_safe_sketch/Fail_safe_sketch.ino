void setup() {
  // put your setup code here, to run once:
  pinMode(P0_17, OUTPUT);
  pinMode(P0_19, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i<3; i++) {
    digitalWrite(P0_17, LOW);
    digitalWrite(P0_19, HIGH);
    delay(500);  
    digitalWrite(P0_17, HIGH);
    digitalWrite(P0_19, LOW);
    delay(500);
  }
  for (int j = 0; j<3; j++) {
    digitalWrite(P0_17, LOW);
    digitalWrite(P0_19, HIGH);
    delay(1000);  
    digitalWrite(P0_17, HIGH);
    digitalWrite(P0_19, LOW);
    delay(1000);
  }
}
