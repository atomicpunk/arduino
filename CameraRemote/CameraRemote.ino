/*
  Camera Remote Control
  Presses remote picture button at an interval
 */

void setup() {
  // button
  pinMode(52, OUTPUT);
  // trigger status
  pinMode(53, OUTPUT);
  // timer
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void rgb(int r, int g, int b, int t)
{
    analogWrite(11, r);
    analogWrite(12, b);
    analogWrite(13, g);
    delay(t);
}

void progressbar(int dtime) {
  int r, g, b, t = dtime/1536;
  for(r = 255, g = 0, b = 0; g < 256; g++)
    rgb(r, g, b, t);
  for(r = 255, g = 255, b = 0; r >= 0; r--)
    rgb(r, g, b, t);
  for(r = 0, g = 255, b = 0; b < 256; b++)
    rgb(r, g, b, t);
  for(r = 0, g = 255, b = 255; g >= 0; g--)
    rgb(r, g, b, t);
  for(r = 0, g = 0, b = 255; r < 256; r++)
    rgb(r, g, b, t);
  for(r = 255, g = 0, b = 255; b >= 0; b--)
    rgb(r, g, b, t);
}

void loop() {
  // off
  digitalWrite(52, LOW);
  digitalWrite(53, LOW);
  progressbar(15000);
  // trigger
  //digitalWrite(52, HIGH);
  //digitalWrite(53, HIGH);
  delay(100);
}
