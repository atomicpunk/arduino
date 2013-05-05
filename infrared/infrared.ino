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
#define IRPIN 0
#define IRMAG(d) ((1000 - (float)(d))/1000)
#define PROTOCOL_UNKNOWN 0
#define PROTOCOL_SHARP 1
#define PROTOCOL PROTOCOL_SHARP
//#define PROTOCOL_DEBUG 1

#if (PROTOCOL == PROTOCOL_SHARP)
#define MAXVALUES 32
#else
#define MAXVALUES 500
#endif

// sections of the IR waveform
enum {
  W0 = 0, // ZERO
  WR,     // RISING EDGE
  W1,     // ONE
  WF      // FALLING EDGE
};

#if (PROTOCOL == PROTOCOL_UNKNOWN)
// start and end time of the IR burst
long start = -1, end = -1;
// positive duty cycle per burst bit
long pdc[MAXVALUES];
// negative duty cycle per burst bit
long ndc[MAXVALUES];
#endif
// burst bit counter
long numvals = 0;
 
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

/*
 * Name: waveform
 * Desc: calculate which phase of a PWM waveform a
 * signal is in given a value representing the fraction
 * of max signal detected.
 * Args:
 *   val: a number between 0 and 1 representing the data
 *   value read on the analog signal line. 1 is the maximum
 *   value detectable, such as 1000 from the ADC
 */
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

/*
 * Name: transition
 * Desc: calculate the timing of a digital waveform by
 * analyzing it at each transition point. From 0 to 1,
 * and 1 to 0.
 * Args:
 *   val: true means transition to 1, false transition to 0
 *   microtime: microsecond timestamp (used for timing info)
 *   millitime: millisecond timestamp (capture a maximum of 1 second of data)
 */
#if (PROTOCOL == PROTOCOL_UNKNOWN)
void transition(bool val, long microtime, long millitime)
{
  static long pmicrotime = -1;

  if((numvals < MAXVALUES)&&((start < 0)||(end - start <= 1000)))
  {
    if(start < 0)
      start = millitime;
    end = millitime;
    
    if(val)
    {
      if(numvals > 0)
        ndc[numvals-1] = microtime - pmicrotime;
      numvals++;
    }
    else
    {
      if(numvals > 0)
        pdc[numvals-1] = microtime - pmicrotime;
    }
  }
  
  pmicrotime = microtime;
}
#else
void transition(bool val, long microtime, long millitime)
{
  static long pmicrotime = -100000;
  static long data = 0, bits = 0;

  if(val)
  {
    // get the negative duty cycle
    long ndc = microtime - pmicrotime;
    // queue up valid bits
    if(ndc < 20000)
    {
      long out = (ndc > 1000)?1:0;
      data = (data << 1) | (out & 0x1);
      bits++;
    }
    if((ndc > 20000)||(bits >= 15))
    {
      if(bits >= 15)
      {
        datapacket(data, 15);
      }
      data = 0;
      bits = 0;
    }
  }
  
  pmicrotime = microtime;
}
#endif

/*
 * Name: datapacket
 * Desc: handle a data packet recieved through an IR signal
 * Args:
 *   data: data packet
 *   length: number of bits in data packet
 */
int datapacket(long data, int length)
{
#if (PROTOCOL == PROTOCOL_SHARP)
  int i, chk, cmd, adr;
  
  // extract the fields  
  chk = data & 0x3;
  adr = (data >> 10) & 0x1F;
  cmd = (data >> 2) & 0xFF;
  if(chk == 1)
    cmd = ((cmd * -1) - 1) & 0xFF;

#ifdef PROTOCOL_DEBUG  
  Serial.print(chk);
  Serial.print(" - ");
  Serial.print(cmd);
  Serial.print(" - ");
  Serial.println(adr);
#endif
  i = keymap(cmd, adr);
  color(i, 1);
  return 0;
#else
  Serial.print("len: ");
  Serial.print(length);
  Serial.print(" data: ");
  Serial.println(data, BIN);
  return 0;
#endif
}

int keymap(int cmd, int addr)
{
#if (PROTOCOL == PROTOCOL_SHARP)
  static int p = 0;
  int pcol[6] = {1, 3, 4, 5, 7, 8};
  if(addr != 16)
    return 0;

  switch(cmd) {
    case 128: //1
      return pcol[0];
    case 64: //2
      return pcol[1];
    case 192: //3
      return pcol[2];
    case 32: //4
      return pcol[3];
    case 160: //5
      return pcol[4];
    case 96: //6
      return pcol[5];
    case 104: //POWER
      p = (p + 1)%6;
      return pcol[p];
    default:
      return 0;
  }
#endif
}

void loop() {
  long microtime = micros(), millitime = millis();
  static long lastcap = -1, pstate = W0;
  // read the data from the pin as raw A2D data
  int raw = analogRead(IRPIN);
  // convert the raw signal into a magnitude
  float mag = IRMAG(raw);
  // find the current waveform state
  int state = waveform(mag);
  int i;

  switch(state) {
    case WR:
    case W1:
      if((pstate == WF)||(pstate == W0))
      {
        // transition on rising edge or flip to 1
        transition(true, microtime, millitime);
#if (PROTOCOL == PROTOCOL_UNKNOWN)
        lastcap = millitime;
        color(4, 1);
#endif
      }
      break;
    case WF:
    case W0:
      if((pstate == WR)||(pstate == W1))
      {
        // transition on falling edge or flip to 0
        transition(false, microtime, millitime);
#if (PROTOCOL == PROTOCOL_UNKNOWN)
        lastcap = millitime;
        color(0, 1);
#endif
      }
      break;
    default:
      Serial.println("UNKNOWN STATE");
      color(0, 0);
  }
  pstate = state;
  
#if (PROTOCOL == PROTOCOL_UNKNOWN)
  // print out the data after the burst has stopped. This is
  // necessary because the 9600 baud console will mess up the
  // timing if we print during the burst
  if((numvals > 0)&&(lastcap >= 0)&&(millitime - lastcap > 200))
  {
      Serial.print("---- captured ");
      Serial.print(end - start);
      Serial.print(" ms -- ");
      Serial.print(numvals);
      Serial.println(" values ----");
      long total = 0;
      long data = 0;
      int bit = 0;
      for(i = 0; i < numvals; i++)
      {
        total += pdc[i]+ndc[i];
#ifdef PROTOCOL_DEBUG
        Serial.print(pdc[i]);
        Serial.print(" ");
        Serial.println(ndc[i]);
#else
        if(ndc[i] < 20000)
        {
          int out = (ndc[i] > 1000)?1:0;
          data |= (out << bit);
          bit++;
        }
        if((ndc[i] > 20000)||(i == numvals - 1))
        {
          datapacket(data, bit);
          data = 0;
          bit = 0;
        }       
#endif
      }
      Serial.print(total);
      Serial.println(" us");
      numvals = 0;
      lastcap = -1;
      start = end = -1;
   }
#endif
}

