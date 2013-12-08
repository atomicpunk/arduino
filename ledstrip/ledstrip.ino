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


#include <avr/pgmspace.h>
#include <Servo.h> 

#if 0 // 1 = Use ATMEGA, 0 = use UNO
#define DATA_1 PORTF   // DATA 1    // for ATMEGA
#define DATA_0 PORTF   // DATA 0    // for ATMEGA
#define STRIP_PINOUT DDRF=0xFF  // for ATMEGA
#define FORMAT_CORRECTION ((j < 10)?2:1)
#define NUM_STRIPS 2
#define SENDSTRIP1 send_strip2
#define SENDSTRIP2 send_strip1
#else 
#define DATA_1 PORTC   // DATA 1    // for UNO
#define DATA_0 PORTC   // DATA 0    // for UNO
#define STRIP_PINOUT (DDRC=0xFF)    // for UNO
#define FORMAT_CORRECTION (2)
#define NUM_STRIPS 2
#define SENDSTRIP1 send_strip2
#define SENDSTRIP2 send_strip2
#endif

// servo and connection data
#define PIN 11
#define SERIAL_BAUD 9600
#define MINMS 620
#define MAXMS 2250

// angle range and presets
#define A_HARDMAX 180
#define A_HARDMIN 0
#define A_SOFTMAX 140
#define A_SOFTMID 95
#define A_SOFTMIN 45
#define A_RESET 96

// pan speed range and presets
#define T_HARDMAX 100
#define T_HARDMIN 1
#define T_SOFTMAX 30
#define T_SOFTMID 10
#define T_SOFTMIN 3

// sweep settings
#define S_DELAY 5
#define S_PAUSE 20000
#define S_CMDCHECK 1000
#define S_INC 1
#define S_DEC (-1*S_INC)

Servo myservo;
String cmdString = "";
int pos=A_RESET, newpos=A_RESET;
int delayval = 1;
bool sweep = false;
int sweepval = S_INC;
long sweepstop[3] = {
  10000, 0, 10000};

PROGMEM const unsigned long pattern_red[10][10]={
  { 0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000  },
};

PROGMEM const unsigned long pattern_blue[10][10]={
  { 0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00  },
};

PROGMEM const unsigned long pattern_green[10][10]={
  { 0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff  },
};

PROGMEM const unsigned long pattern_white[10][10]={
  { 0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff  },
};

PROGMEM const unsigned long pattern_comet1[][10]={
  { 0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000  },
  { 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff  },
};

PROGMEM const unsigned long pattern_comet2[][10]={
  { 0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000  },
  { 0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000  },
  { 0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000  },
  { 0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000  },
  { 0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff  },
  { 0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000  },
  { 0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000  },
  { 0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000  },
  { 0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000  },
  { 0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff  },
};

PROGMEM const unsigned long pattern_comet3[][10]={
  { 0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff  },
  { 0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x444444  },
  { 0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0xffffff,0x444444,0x111111  },
  { 0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0xffffff,0x444444,0x111111,0x000000  },
  { 0x000000,0x000000,0x111111,0x444444,0xffffff,0xffffff,0x444444,0x111111,0x000000,0x000000  },
  { 0x000000,0x000000,0x111111,0x444444,0xffffff,0xffffff,0x444444,0x111111,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0xffffff,0x444444,0x444444,0xffffff,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0xffffff,0x444444,0x111111,0x111111,0x444444,0xffffff,0x000000,0x000000  },
  { 0x000000,0xffffff,0x444444,0x111111,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000  },
  { 0xffffff,0x444444,0x111111,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff  },
};

PROGMEM const unsigned long pattern_thanksgiving1[][10]={
  { 0xff7f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff7f00  },
  { 0x5f2f00,0xff7f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff7f00,0x5f2f00  },
  { 0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000,0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00  },
  { 0x000000,0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x000000  },
  { 0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000  },
  { 0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0xff7f00,0x5f2f00,0x5f2f00,0xff7f00,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000  },
  { 0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0x000000  },
  { 0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000,0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00  },
};

PROGMEM const unsigned long pattern_christmas1[][10]={
  { 0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000  },
  { 0x5f0000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x5f0000  },
  { 0x1f0000,0x5f0000,0xff0000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x5f0000,0x1f0000  },
  { 0x000000,0x1f0000,0x5f0000,0xff0000,0x000000,0x000000,0xff0000,0x5f0000,0x1f0000,0x000000  },
  { 0x000000,0x000000,0x1f0000,0x5f0000,0xff0000,0xff0000,0x5f0000,0x1f0000,0x000000,0x000000  },
  { 0x000000,0x000000,0x1f0000,0x5f0000,0xff0000,0xff0000,0x5f0000,0x1f0000,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0xff0000,0x5f0000,0x5f0000,0xff0000,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0xff0000,0x5f0000,0x1f0000,0x1f0000,0x5f0000,0xff0000,0x000000,0x000000  },
  { 0x000000,0xff0000,0x5f0000,0x1f0000,0x000000,0x000000,0x1f0000,0x5f0000,0xff0000,0x000000  },
  { 0xff0000,0x5f0000,0x1f0000,0x000000,0x000000,0x000000,0x000000,0x1f0000,0x5f0000,0xff0000  },
};

PROGMEM const unsigned long pattern_christmas2[][10]={
  { 0x007f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x007f00  },
  { 0x002f00,0x007f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x007f00,0x002f00  },
  { 0x000f00,0x002f00,0x007f00,0x000000,0x000000,0x000000,0x000000,0x007f00,0x002f00,0x000f00  },
  { 0x000000,0x000f00,0x002f00,0x007f00,0x000000,0x000000,0x007f00,0x002f00,0x000f00,0x000000  },
  { 0x000000,0x000000,0x000f00,0x002f00,0x007f00,0x007f00,0x002f00,0x000f00,0x000000,0x000000  },
  { 0x000000,0x000000,0x000f00,0x002f00,0x007f00,0x007f00,0x002f00,0x000f00,0x000000,0x000000  },
  { 0x000000,0x000000,0x000000,0x007f00,0x002f00,0x002f00,0x007f00,0x000000,0x000000,0x000000  },
  { 0x000000,0x000000,0x007f00,0x002f00,0x000f00,0x000f00,0x002f00,0x007f00,0x000000,0x000000  },
  { 0x000000,0x007f00,0x002f00,0x000f00,0x000000,0x000000,0x000f00,0x002f00,0x007f00,0x000000  },
  { 0x007f00,0x002f00,0x000f00,0x000000,0x000000,0x000000,0x000000,0x000f00,0x002f00,0x007f00  },
};

PROGMEM const unsigned long pattern_rainbow[10][10]={
  { 0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000  },
  { 0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000  },
  { 0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000  },
  { 0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff  },
  { 0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff  },
  { 0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff  },
  { 0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00  },
  { 0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00  },
  { 0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00  },
  { 0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000  },
};

unsigned long halloween_basic[] =    { 0x707f00,0xdf6f00,0x9f4f00,0x5f2f00,0x1f0f00,0x070300,0x1f0f00,0x5f2f00,0x9f4f00,0xdf6f00 };
unsigned long thanksgiving_basic[] = { 0xff7f00,0xdf6f00,0x9f4f00,0x5f2f00,0x1f0f00,0x070300,0x1f0f00,0x5f2f00,0x9f4f00,0xdf6f00 };
unsigned long christmas_basic[] =    { 0xff0000,0x006f00,0x9f0000,0x002f00,0x1f0000,0x000300,0x1f0000,0x002f00,0x9f0000,0x006f00 };
unsigned long pattern[10][NUM_STRIPS*10]={};
unsigned long format_correction(unsigned long val, int fmt);

/*******************************************************************************
 * Function Name  : send_1M_pattern
 * Description    : Transmit pattern to whole 1 meter strip
 *                  
 * Input          : pointer to ROM pattern; pattern length; frame rate
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void send_1M_pattern(const unsigned long data[][10], int pattern_no, int frame_rate)
{
  int i=0;
  int j=0;
  uint32_t temp_data;
  unsigned long val;

  loop2();
  for (i=0;i<pattern_no;i++)
  {
    noInterrupts();
    for (j=0;j<10;j++)
    {
      temp_data=pgm_read_dword_near(&data[i][j]);
      SENDSTRIP1(temp_data, 0x01, 0xFE);
    }
#if NUM_STRIPS > 1
    for (j=0;j<10;j++)
    {
      temp_data=pgm_read_dword_near(&data[i][j]);
      SENDSTRIP2(temp_data, 0x02, 0xFD);
    }
#endif
    interrupts();

    loop2();
    delay(frame_rate);
  }
}

void send_pattern(int frame_rate)
{
  int i,j;

  loop2();
  for (i=0;i<10;i++)
  {
    noInterrupts();
    for (j=0;j<(NUM_STRIPS*10);j++)
      send_strip(pattern[i][j], (j<10)?0x01:0x02, (j<10)?0xFE:0xFD);
    interrupts();
    loop2();
    delay(frame_rate);
  }
}

unsigned long format_correction(unsigned long val, int fmt)
{
  unsigned long r = (val >> 16) & 0xff, 
  g = (val >> 8) & 0xff,
  b = (val >> 0) & 0xff;
  switch(fmt) {
  case 1: // RBG
    return (r << 16) | (b << 8) | g;
  case 2: // GBR
    return (g << 16) | (b << 8) | r;
  }
  return val;
}

void setup_pattern(unsigned long basic[], int format) {
  int i,j,k;
  unsigned long frame[10], temp;

  for (i=0;i<10;i++)
    frame[i] = basic[i];

  for (i=0;i<10;i++)
  {
    for (j=0;j<(NUM_STRIPS*10);j++)
      pattern[i][j] = format_correction(frame[j%10], FORMAT_CORRECTION);

    switch(format) {
    case 0: // step 10 times, move towards board
      temp = frame[0];
      for (k=0;k<9;k++)
        frame[k] = frame[k+1];
      frame[9] = temp;
      break;
    case 1: // step 10 times, move away from board
      temp = frame[9];
      for (k=9;k>0;k--)
        frame[k] = frame[k-1];
      frame[0] = temp;
      break;
    }
  }
}

bool isNumber(String val)
{
  int i;
  for(i = 0; i < val.length(); i++)
    if(!isDigit(val.charAt(i)))
      return false;
  return true;
}

int a_to_ms(unsigned long a)
{
  return ((a*(MAXMS-MINMS))/A_HARDMAX)+MINMS;
}

void runCommand()
{
  char cmd = cmdString.charAt(0);
  String data = cmdString.substring(1);
  int temp;

  switch(cmd) {
  case 'a':
    if((data == "min")||(data == "left"))
      temp = A_SOFTMIN;
    else if((data == "max")||(data == "right"))
      temp = A_SOFTMAX;
    else if((data == "mid")||(data == "center"))
      temp = A_SOFTMID;
    else if(isNumber(data))
      temp = data.toInt();
    else
    {
      Serial.print("Malformed command: ");
      Serial.println(cmdString);
      break;
    }

    if((temp >= A_HARDMIN)&&(temp <= A_HARDMAX))
    {
      sweep = false;
      newpos = temp;
      if(!myservo.attached())
      {
        Serial.println("Attach");
        myservo.attach(PIN);
      }
    }
    else
    {
      Serial.print(temp);
      Serial.println("deg out of range");
    }
    break;
  case 't':
    if(data == "slow")
      temp = T_SOFTMAX;
    else if(data == "fast")
      temp = T_SOFTMIN;
    else if(data == "norm")
      temp = T_SOFTMID;
    else if(isNumber(data))
      temp = data.toInt();
    else
    {
      Serial.print("Malformed command: ");
      Serial.println(cmdString);
      break;
    }

    if((temp >= T_HARDMIN)&&(temp <= T_HARDMAX))
    {
      delayval = temp;
      Serial.print("New delayval: ");
      Serial.println(delayval);
    }
    else
    {
      Serial.print(temp);        
      Serial.println("ms out of range");
    }
    break;
  case 'i':
    Serial.print("Angle: ");      
    Serial.println(pos);
    Serial.print("Delay: ");      
    Serial.println(delayval);
    break;
  case 's':
    if(cmdString == "s")
    {
      sweep = !sweep;
      if(sweep)
      {
        Serial.println("sweep started");
        myservo.attach(PIN);
        delayval = S_DELAY;
        if(pos >= A_SOFTMAX)
        {
          newpos = A_SOFTMAX-1;
          sweepval = S_DEC;
        }
        else if(pos <= A_SOFTMIN)
        {
          newpos = A_SOFTMIN+1;
          sweepval = S_INC;
        }
        else if(pos == A_SOFTMID)
        {
          newpos += sweepval;
        }
      }
      else
      {
        Serial.println("sweep stopped");     
        myservo.detach();
        delayval = T_SOFTMID;
      }
    }
    else
    {
      Serial.print("Malformed command: ");
      Serial.println(cmdString);
    }
    break;
  default:
    Serial.print("Unknown command: ");
    Serial.println(cmdString);
  }
  cmdString = "";
}

void checkForCommand()
{
  // assemble command char by char from serial
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if((c == '=')||(c == '\n'))
    {
      runCommand();
    }
    else
    {
      cmdString += c;
    }
  }  
}

void runSweep()
{
  if((pos >= A_SOFTMAX)||(pos <= A_SOFTMIN)||(pos == A_SOFTMID))
  {
    long sweepdelay;
    if(pos >= A_SOFTMAX)
    {
      sweepval *= -1;
      sweepdelay = sweepstop[2]; // right view time
    }
    else if(pos <= A_SOFTMIN)
    {
      sweepval *= -1;
      sweepdelay = sweepstop[0]; // left view time
    }
    else
    {
      sweepdelay = sweepstop[1]; // center view time
    }
    if(sweepdelay > 0)
    {
      myservo.detach();
      for(int i = 0; i < sweepdelay; i += S_CMDCHECK)
      {
        checkForCommand();
        if(sweep)
        {
          delay(S_CMDCHECK);
        }
      }
      if(sweep)
      {
        myservo.attach(PIN);
      }
    }
  }
  if(sweep)
  {
    newpos += sweepval;
  }
}

void setup() {                
  STRIP_PINOUT;        // set output pin - DEBUG: should auto detect which mother board for use
  reset_strip(0xFE);
#if NUM_STRIPS > 1
  reset_strip(0xFD);
#endif
  setup_pattern(christmas_basic, 1);
  Serial.begin(SERIAL_BAUD);
  myservo.attach(PIN);
  delay(1000);
  myservo.detach();
}

void loop2() 
{
  if(sweep)
  {
    runSweep();
  }

  checkForCommand();

  if ((newpos != pos) && myservo.attached()) {
    int newval = a_to_ms(newpos);
    int val = a_to_ms(pos);
    if(!sweep)
    {
      Serial.print(pos);
      Serial.print(" -> ");
      Serial.println(newpos);
    }
    for(; newval < val; val--)
    {
      myservo.writeMicroseconds(val);
      delay(delayval);
    }
    for(; newval > val; val++)
    {
      myservo.writeMicroseconds(val);
      delay(delayval);
    }
    pos = newpos;
    if(!sweep)
    {
      Serial.println("Detach");
      myservo.detach();
    }
  }
}

void loop()
{
  send_pattern(100);
  send_pattern(100);
  send_pattern(100);
  send_pattern(100);
  send_1M_pattern(pattern_christmas1, 10, 100);
  send_1M_pattern(pattern_christmas2, 10, 100);
  send_1M_pattern(pattern_christmas1, 10, 100);
  send_1M_pattern(pattern_christmas2, 10, 100);
}

/*******************************************************************************
 * Function Name  : send_strip
 * Description    : Transmit 24 pulse to LED strip
 *                  
 * Input          : 24-bit data for the strip
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void send_strip2(uint32_t temp, uint32_t one, uint32_t zero)
{
  int i;
  unsigned long j=0x800000;
  unsigned long r = (temp >> 16) & 0xff, 
  g = (temp >> 8) & 0xff,
  b = (temp >> 0) & 0xff;
  unsigned long data = (g << 16) | (b << 8) | r;

  for (i=0;i<24;i++)
  {
    if (data & j)
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");

      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");  
      __asm__("nop\n\t");        
      /*----------------------------*/
      DATA_0 &= zero;
    }
    else
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      DATA_0 &= zero;
      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");      
      /*----------------------------*/
    }

    j>>=1;
  }
}

void send_strip1(uint32_t temp, uint32_t one, uint32_t zero)
{
  int i;
  unsigned long j=0x800000;
  unsigned long r = (temp >> 16) & 0xff, 
  g = (temp >> 8) & 0xff,
  b = (temp >> 0) & 0xff;
  unsigned long data = (r << 16) | (b << 8) | g;

  for (i=0;i<24;i++)
  {
    if (data & j)
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");

      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");  
      __asm__("nop\n\t");        
      /*----------------------------*/
      DATA_0 &= zero;
    }
    else
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      DATA_0 &= zero;
      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");      
      /*----------------------------*/
    }

    j>>=1;
  }
}

void send_strip(uint32_t data, uint32_t one, uint32_t zero)
{
  int i;
  unsigned long j=0x800000;

  for (i=0;i<24;i++)
  {
    if (data & j)
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");

      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");  
      __asm__("nop\n\t");  
      __asm__("nop\n\t");        
      /*----------------------------*/
      DATA_0 &= zero;
    }
    else
    {
      DATA_1 |= one;
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");    
      DATA_0 &= zero;
      /*----------------------------*/
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      __asm__("nop\n\t");      
      /*----------------------------*/
    }

    j>>=1;
  }
}

/*******************************************************************************
 * Function Name  : reset_strip
 * Description    : Send reset pulse to reset all color of the strip
 *                  
 * Input          : None
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void reset_strip(uint32_t zero)
{
  DATA_0 &= zero;
  delayMicroseconds(20);
}


