/* 
 * ROYGBIV - lighting effects for RGB LEDs
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
 
#define NUMCOLORS 7
#define COLORTIME 1000
#define COLORDELAY 100
#define INCREMENTS (COLORTIME/COLORDELAY)
//#define HISTORY INCREMENTS
#define HISTORY 1
//#define D1 ((HISTORY/2)-1)
//#define D2 (HISTORY-1)
#define D1 0
#define D2 0

int colorwheel[NUMCOLORS][3] = {
  {255, 127,   0}, // orange
  {255, 255,   0}, // yellow
  {  0, 255,   0}, // green
  {  0,   0, 255}, // blue
  {127,   0, 255}, // indigo
  {255,   0, 255}, // violet
  {255,   0,   0}  // red (first color in loop)
};

void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

int history[HISTORY][3];

void rgb(int r, int g, int b)
{
    int i, j;
    
    for(i = HISTORY-1; i > 0; i--)
        for(j = 0; j < 3; j++)
            history[i][j] = history[i-1][j];

    history[0][0] = r;
    history[0][1] = b;
    history[0][2] = g;

    analogWrite(5, history[D2][0]);
    analogWrite(6, history[D2][1]);
    analogWrite(7, history[D2][2]);
    analogWrite(8, history[D1][0]);
    analogWrite(9, history[D1][1]);
    analogWrite(10, history[D1][2]);
    analogWrite(11, r);
    analogWrite(12, b);
    analogWrite(13, g);
    delay(COLORDELAY);
}

void loop() {
  int i, j, l;
  float r, g, b, dR, dG, dB;
  
  for(i = 0; i < NUMCOLORS; i++)
  {
    l = (i > 0)?(i - 1):(NUMCOLORS-1);
    r = colorwheel[l][0];
    g = colorwheel[l][1];
    b = colorwheel[l][2];
    dR = (colorwheel[i][0] - r)/INCREMENTS;
    dG = (colorwheel[i][1] - g)/INCREMENTS;
    dB = (colorwheel[i][2] - b)/INCREMENTS;
    for(j = 0; j < INCREMENTS; j++)
    {
      rgb((int)r, (int)g, (int)b);
      r += dR;
      g += dG;
      b += dB;
    }
  }
}

