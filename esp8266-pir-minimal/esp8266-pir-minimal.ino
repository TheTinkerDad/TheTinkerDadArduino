void setup() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
}

void loop() {
}

void IntCallback()
{
  if (digitalRead(D2) == HIGH) 
    digitalWrite(D5, HIGH);
  else 
    digitalWrite(D5, LOW);
}
