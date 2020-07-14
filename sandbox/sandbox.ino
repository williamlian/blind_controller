void setup() {
  pinMode(14, INPUT);
  pinMode(16, OUTPUT);
  
  digitalWrite(16, HIGH);

  
  pinMode(5, OUTPUT);
}

int t = LOW;
void loop() {
  //digitalWrite(16, digitalRead(14) == HIGH ? LOW : HIGH);

  digitalWrite(5, t);
  t = t == HIGH ? LOW : HIGH;
  delay(200);
}
