void setup() {
  Serial.begin(9600);
  pinMode(A11, INPUT);
}
void loop() {
  int forceValue = analogRead(A11);
  int forceConstriction = map(forceValue, 0, 40);
  if (forceConstriction > 40) { 
    // game action meowmeow meow
  }
  delay(100); 
}


