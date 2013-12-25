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
typedef void (*call_t)();
struct thread_t {
  unsigned long lasttime;
  unsigned long timeslice;
  call_t setupfunc;
  call_t loopfunc;
};

#define BOARD_UNO 1
#include "/home/tebrandt/workspace/arduino/ledstrip/ledstrip.h"

//#include <Servo.h>
//#include "/home/tebrandt/workspace/arduino/Sweep/Sweep.h"

//#include <VirtualWire.h>
//#define RADIOPIN 1

struct thread_t threads[] = {
  {0, FRAMEDELAY, setupLedStrip, loopLedStrip},
//  {0, 240, setupSweep, loopSweep},
};
#define NUM_THREADS (sizeof(threads)/sizeof(struct thread_t))

void setup() {
  int i;
  for(i = 0; i < NUM_THREADS; i++) {
    threads[i].lasttime = millis();
    threads[i].setupfunc();
  }
//  vw_set_ptt_inverted(true); // Required for DR3100
//  vw_setup(1200);	 // Bits per sec
//  vw_set_tx_pin(RADIOPIN); 
}

//const char *synccmd = "b";
void loop()
{
  int i;
  unsigned long time = millis();
  for(i = 0; i < NUM_THREADS; i++) {
    if(time - threads[i].lasttime >= threads[i].timeslice) {
      threads[i].loopfunc();
      threads[i].lasttime = time;
//      if(ledframe_index == 0) {
//        vw_send((uint8_t *)synccmd, 1);
//        vw_wait_tx();
//      }
    }
  }
}

