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
#define LEDS 7
int pins[LEDS] = {13,11,9,7,5,3,1};

void light(int level)
{
  int i;
  for(i = 0; i < LEDS; i++) {
    if(i < level)
      digitalWrite(pins[i], HIGH);
    else
      digitalWrite(pins[i], LOW);
  }
}

void setup() {
  int i;
  Serial.begin(9600);
  for(i = 0; i < LEDS; i++)
    pinMode(pins[i], OUTPUT);
  pinMode(RADIOPIN, INPUT);
  light(0);
  vw_set_ptt_inverted(true);
  vw_setup(1200);
  //vw_set_rx_pin(RADIOPIN);
  vw_rx_start();
  Serial.println("Leonardo READY");
}

void loop()
{
  int raw = digitalRead(RADIOPIN);
  light(raw);
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    light(5);
    Serial.print("Got: ");
    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

