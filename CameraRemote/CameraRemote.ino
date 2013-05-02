/*
 * CameraRemote - Servo based Camera Remote Control
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
