/***************************************************************************/    
//            Hardware: RadioShack Tricolor LED Strip
//            Arduino IDE: Arduino-1.0
//            Date:      April 17, 2013
//            Copyright© 2013 RadioShack Corporation
//tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt m//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, see <http://www.gnu.org/licenses/>
//
/*****************************************************************************/

#include <avr/pgmspace.h>

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

PROGMEM const unsigned long pattern_red[10][10]={
  {0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff0000},
};

PROGMEM const unsigned long pattern_blue[10][10]={
  {0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00},
};

PROGMEM const unsigned long pattern_green[10][10]={
  {0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x0000ff},
};

PROGMEM const unsigned long pattern_white[10][10]={
  {0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff},
};

PROGMEM const unsigned long pattern_comet1[][10]={
  {0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff},
};

PROGMEM const unsigned long pattern_comet2[][10]={
  {0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000},
  {0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000},
  {0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000},
  {0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000},
  {0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff},
  {0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000},
  {0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000},
  {0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000},
  {0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000},
  {0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0x111111,0x444444,0xffffff},
};

PROGMEM const unsigned long pattern_comet3[][10]={
  {0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff},
  {0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xffffff,0x444444},
  {0x111111,0x444444,0xffffff,0x000000,0x000000,0x000000,0x000000,0xffffff,0x444444,0x111111},
  {0x000000,0x111111,0x444444,0xffffff,0x000000,0x000000,0xffffff,0x444444,0x111111,0x000000},
  {0x000000,0x000000,0x111111,0x444444,0xffffff,0xffffff,0x444444,0x111111,0x000000,0x000000},
  {0x000000,0x000000,0x111111,0x444444,0xffffff,0xffffff,0x444444,0x111111,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0xffffff,0x444444,0x444444,0xffffff,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0xffffff,0x444444,0x111111,0x111111,0x444444,0xffffff,0x000000,0x000000},
  {0x000000,0xffffff,0x444444,0x111111,0x000000,0x000000,0x111111,0x444444,0xffffff,0x000000},
  {0xffffff,0x444444,0x111111,0x000000,0x000000,0x000000,0x000000,0x111111,0x444444,0xffffff},
};

PROGMEM const unsigned long pattern_thanksgiving1[][10]={
  {0xff7f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff7f00},
  {0x5f2f00,0xff7f00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0xff7f00,0x5f2f00},
  {0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000,0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00},
  {0x000000,0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x000000},
  {0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000},
  {0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0xff7f00,0x5f2f00,0x5f2f00,0xff7f00,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x1f0f00,0x5f2f00,0xff7f00,0x000000,0x000000},
  {0x000000,0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00,0x000000},
  {0xff7f00,0x5f2f00,0x1f0f00,0x000000,0x000000,0x000000,0x000000,0x1f0f00,0x5f2f00,0xff7f00},
};

PROGMEM const unsigned long pattern_rainbow[10][10]={
  {0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000},
  {0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000},
  {0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000},
  {0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff},
  {0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff},
  {0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00,0x0000ff},
  {0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00,0x00ff00},
  {0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00,0xffff00},
  {0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000,0xff7f00},
  {0xff7f00,0xffff00,0x00ff00,0x0000ff,0x6f00ff,0x8f00ff,0x000000,0x000000,0x000000,0xff0000},
};

unsigned long halloween_basic[] = {0x707f00,0xdf6f00,0x9f4f00,0x5f2f00,0x1f0f00,0x070300,0x1f0f00,0x5f2f00,0x9f4f00,0xdf6f00};
unsigned long thanksgiving_basic[] = {0xff7f00,0xdf6f00,0x9f4f00,0x5f2f00,0x1f0f00,0x070300,0x1f0f00,0x5f2f00,0x9f4f00,0xdf6f00};
unsigned long christmas_basic[] = {0xff0000,0x006f00,0x9f0000,0x002f00,0x1f0000,0x000300,0x1f0000,0x002f00,0x9f0000,0x006f00};
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

    delay(frame_rate);
  }
}

void send_pattern(int frame_rate)
{
  int i,j;

  for (i=0;i<10;i++)
  {
    noInterrupts();
    for (j=0;j<(NUM_STRIPS*10);j++)
      send_strip(pattern[i][j], (j<10)?0x01:0x02, (j<10)?0xFE:0xFD);
    interrupts();
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

void setup() {                
  STRIP_PINOUT;        // set output pin - DEBUG: should auto detect which mother board for use
  reset_strip(0xFE);
  #if NUM_STRIPS > 1
  reset_strip(0xFD);
  #endif
  setup_pattern(thanksgiving_basic, 1);
}

void loop()
{
  send_pattern(100);
  send_pattern(100);
  send_pattern(100);
  send_pattern(100);
  send_1M_pattern(pattern_thanksgiving1, 10, 100);
  send_1M_pattern(pattern_thanksgiving1, 10, 100);
  send_1M_pattern(pattern_thanksgiving1, 10, 100);
  send_1M_pattern(pattern_thanksgiving1, 10, 100);
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

