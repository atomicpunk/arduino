/* 
 * ircontrol - remote control emulator
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

#define NUMCOLORS (sizeof(colorwheel)/(sizeof(int)*3))
#define COLOR(i) rgb(colorwheel[i][0], colorwheel[i][1], colorwheel[i][2])
#define ACTIVE LOW
#define INACTIVE HIGH
#define IRPIN 7
#define PDC 320     // positive duty cycle us
#define NDC0 680    // negative duty cycle us for ZERO
#define NDC1 1680   // negative duty cycle us for ONE
#define COMPLETE 45 // negative duty cycle ms for cmd COMPLETE

// sharp remote commands
enum {
  BUTTON_ONE=0,
  BUTTON_TWO,
  BUTTON_THREE,
  BUTTON_POWER,
  BUTTON_DISPLAY
};
const long cmdlist[][3] = {
  {0x4202, 0x41FD, 15},
  {0x4102, 0x42FD, 15},
  {0x4302, 0x40FD, 15},
  {0x41A2, 0x425D, 15},
  {0x4362, 0x409D, 15}
};

int colorwheel[][3] = {
  {  0,   0,   0}, // 0: black (off)
  {255,   0,   0}, // 1: red
  {255, 127,   0}, // 2: orange
  {255, 255,   0}, // 3: yellow
  {  0, 255,   0}, // 4: green
  {  0,   0, 255}, // 5: blue
  {127,   0, 255}, // 6: indigo
  {255,   0, 255}, // 7: violet
  {  0, 255, 255}, // 8: seagreen
  {255, 255, 255}  // 9: white
};

void setup() {
  pinMode(IRPIN, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(IRPIN, INACTIVE);
}

void rgb(int r, int g, int b)
{
    analogWrite(13, r);
    analogWrite(11, b);
    analogWrite(10, g);
}

void sendCmd(long data, int bits)
{
  long i, bit;

  digitalWrite(IRPIN, INACTIVE);
  for(i = bits - 1; i >= 0; i--)
  {
    bit = (data >> i) & 0x1;
    digitalWrite(IRPIN, ACTIVE);
    delayMicroseconds(PDC);
    digitalWrite(IRPIN, INACTIVE);
    if(bit)
      delayMicroseconds(NDC1);
    else
      delayMicroseconds(NDC0);
  }
  digitalWrite(IRPIN, ACTIVE);
  delayMicroseconds(PDC);
  digitalWrite(IRPIN, INACTIVE);
  delay(COMPLETE);
//  digitalWrite(IRPIN, ACTIVE);
//  delayMicroseconds(PDC);
//  digitalWrite(IRPIN, INACTIVE);
}

void issueCmd(int cmd)
{
  sendCmd(cmdlist[cmd][0], cmdlist[cmd][2]);
  sendCmd(cmdlist[cmd][1], cmdlist[cmd][2]);
  sendCmd(cmdlist[cmd][0], cmdlist[cmd][2]);
}

void holdCmd(int cmd, long ms)
{
  long time = millis();
  
  sendCmd(cmdlist[cmd][0], cmdlist[cmd][2]);
  while(millis() - time < ms)
  {
    sendCmd(cmdlist[cmd][1], cmdlist[cmd][2]);
    sendCmd(cmdlist[cmd][0], cmdlist[cmd][2]);
  }
}

void cmdtest(int cmd, int cmd_ms, int del_ms, int color)
{
  delay(del_ms);
  COLOR(color);
  if(cmd_ms > 0)
    holdCmd(cmd, cmd_ms);
  else
    issueCmd(cmd);
  COLOR(0);
}

void loop() {
  cmdtest(BUTTON_ONE, 0, 2000, 1);
  cmdtest(BUTTON_TWO, 0, 2000, 3);
  cmdtest(BUTTON_THREE, 0, 2000, 4);
  cmdtest(BUTTON_DISPLAY, 0, 2000, 1);
  cmdtest(BUTTON_POWER, 1000, 2000, 5);
}

