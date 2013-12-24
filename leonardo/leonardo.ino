/*
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

#include <Arduino.h>
#include <VirtualWire.h>

#define RADIOPIN 0
#define PIN_R 13
#define PIN_G 11
#define PIN_B 10

int colorwheel[7][3] = {
  {255, 127,   0}, // orange
  {255, 255,   0}, // yellow
  {  0, 255,   0}, // green
  {  0,   0, 255}, // blue
  {127,   0, 255}, // indigo
  {255,   0, 255}, // violet
  {255,   0,   0}  // red                                                                                         
};

void rgb(int r, int g, int b)
{
    analogWrite(PIN_R, r);
    analogWrite(PIN_G, b);
    analogWrite(PIN_B, g);
}

void setup() {
  pinMode(RADIOPIN, INPUT);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
}

void loop()
{
  int val = digitalRead(RADIOPIN);
  if(val)
    rgb(val, 0, 0);
  else
    rgb(0, val, 0);
}

