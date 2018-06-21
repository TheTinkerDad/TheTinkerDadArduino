void setup() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
}

void loop() {
}

void IntCallback()
{
  if (digitalRead(D2) == HIGH) 
    digitalWrite(D0, LOW);
  else 
    digitalWrite(D0, HIGH);
}
