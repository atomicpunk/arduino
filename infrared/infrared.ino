/* 
 * Infrared - program to use IR sensor to record remote output timing
 * Copyright 2012 Todd Brandt <tebrandt@frontier.com>
 *
 * This program is free software; you may redistribute it and/or modify it
 * under the same terms as Perl itself.
 *    trancearoundtheworld mp3 archive sync utility
 *    Copyright (C) 2012 Todd Brandt <tebrandt@frontier.com>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define SERIAL_BAUD 9600
#define NUMCOLORS (sizeof(colorwheel)/(sizeof(int)*3))
#define IRPIN 12
#define IRMAG(d) ((995 - (float)(d))/995)

int colorwheel[][3] = {
  {  0,   0,   0}, // 0: black (off)
  {255,   0,   0}, // 1: red
  {255, 127,   0}, // 2: orange
  {255, 255,   0}, // 3: yellow
  {  0, 255,   0}, // 4: green
  {  0,   0, 255}, // 5: blue
  {127,   0, 255}, // 6: indigo
  {255,   0, 255}  // 7: violet
};

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(IRPIN, INPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void rgb(int r, int g, int b)
{
  analogWrite(13, r);
  analogWrite(12, b);
  analogWrite(11, g);
}

void color(int cidx, float mag)
{
  if(mag < 0)
    mag = 0;
  rgb((int)(mag*(float)colorwheel[cidx][0]),
      (int)(mag*(float)colorwheel[cidx][1]),
      (int)(mag*(float)colorwheel[cidx][2]));
}

enum {
  W0 = 0, // ZERO
  WR,     // RISING EDGE
  W1,     // ONE
  WF      // FALLING EDGE
};

int waveform(float val)
{
  static float pval = val;
  static int pres = W0;
  int res = W0;

  if((val - pval) > 0.01)
  {
    // RISING EDGE
    if(pres == W1) { 
      res = W1;
    } else {
      res = WR;
    }
  }
  else if((pval - val) > 0.01)
  {
    // FALLING EDGE
    if(pres == W0) {
      res = W0;
    } else {
      res = WF;
    }
  }
  else
  {
    // ZERO or ONE
    res = (val > 0.01)?W1:W0;
  }
  
  pval = val;
  pres = res;
  return res;
}

#define MAXVALUES 1000
int pdc[MAXVALUES];
int ndc[MAXVALUES];
int numvalues = 0;

void loop() {
  static long microtime = -1, millitime = -1;
  int raw = analogRead(IRPIN);
  float mag = IRMAG(raw);
  int state = waveform(mag);
  int i;
  
  switch(state) {
    case WR:
    case W1:
      if(microtime < 0)
      {
        microtime = micros();
        color(4, 1);
      }
      break;
    case WF:
    case W0:
      if(microtime >= 0)
      {
        if(numvalues < MAXVALUES)
        {
          numvalues++;
          pdc[numvalues-1] = micros() - microtime;
        }
        millitime = millis();
        microtime = -1;
        color(0, 1);
      }
      else if((millitime > 0)&&(millis() - millitime > 200))
      {
        Serial.print("---- captured ");
        Serial.print(numvalues);
        Serial.println(" values ----");
        for(i = 0; i < numvalues; i++)
        {
          Serial.print(pdc[i]);
          if(i == numvalues - 1)
            Serial.println("");
          else if((i+1)%20 == 0)
            Serial.println(",");
          else
            Serial.print(", ");
        }
        numvalues = 0;
        millitime = -1;
      }
      break;
    default:
      Serial.println("UNKNOWN STATE");
      color(0, 0);
  }
}

