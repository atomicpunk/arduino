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
#define PIN_G 10
#define PIN_B 11

int colorwheel[7][3] = {
  {255,   0,   0}, // red 
  {255, 127,   0}, // orange
  {255, 255,   0}, // yellow
  {  0, 255,   0}, // green
  {  0,   0, 255}, // blue
  {127,   0, 255}, // indigo
  {255,   0, 255}, // violet
};

void rgb(int r, int g, int b)
{
    analogWrite(PIN_R, r);
    analogWrite(PIN_G, g);
    analogWrite(PIN_B, b);
}

void color(int i)
{
    analogWrite(PIN_R, colorwheel[i][0]);
    analogWrite(PIN_G, colorwheel[i][1]);
    analogWrite(PIN_B, colorwheel[i][2]);
}

void setup() {
  rgb(0, 0, 0);
  Serial.begin(9600);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  vw_set_ptt_inverted(true);
  vw_setup(1200);
  vw_set_rx_pin(RADIOPIN);
  vw_rx_start();
}

void loop()
{
    int i;
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen))
    {
        color(3);
        for(i = 0; i < buflen; i++) {
          Serial.print((char)buf[i]);
        }
        Serial.println("");
    } else {
        rgb(0, 0, 0);
    }
}

