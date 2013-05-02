/*
  Spectrum
  Playing with RGB LEDs
 */
 
void setup() {
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void rgb(int r, int g, int b)
{
    analogWrite(11, r);
    analogWrite(12, b);
    analogWrite(13, g);
}

void loop() {
  int i, j, k;
  for(i = 0; i < 100; i++)
    for(j = 0; j < 100; j++)
      for(k = 0; k < 100; k++)
      {
        rgb(i, j, k);
        delay(15);
      }
}
