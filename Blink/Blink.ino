/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(52, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(52, LOW);
  delay(15000);
  digitalWrite(52, HIGH);
  delay(1000);
}
