// Very basic traffic light for kids' toys

void setup() {

  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);

  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);

  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
}

void loop() {

  noTone(D5);
  
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

  delay ( 5000 );

  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

  for (int i = 0; i < 3; i++) {
    tone(D5, 261, 500);
    digitalWrite(D3, HIGH);
    delay ( 500 );
    digitalWrite(D3, LOW);
    delay ( 500 );
  }

  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);

  tone(D5,261, 1000);

  delay ( 18000 );
}
