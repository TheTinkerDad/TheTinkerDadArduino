void setup() {

  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);

  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);

  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

  delay ( 12000 );

  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, LOW);

  delay ( 2000 );

  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);

  delay ( 18000 );

  digitalWrite(D2, LOW);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, LOW);

  delay ( 3000 );
}
