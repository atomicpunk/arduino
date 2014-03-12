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

#include <VirtualWire.h>
#define RADIOPIN 0

#if 0
//#include <Servo.h>
#define SERVO_ALT_PIN 11
#define SERVO_AZI_PIN 10
#define SERVO_CLAW_PIN 11
#define SERVO_WRIST_PIN 10
#define ALTMIN 700  // 0 degrees (down)
#define ALTMAX 2000 // 120 degrees (forward/up)
#define ALTANGMAX 125
#define AZIMIN 880  // 0 degrees (right)
#define AZIMAX 2480 // 180 degrees (left)
#define AZIANGMAX 180
#define CLAWMIN 600  // 0 degrees (down)
#define CLAWMAX 2200 // 120 degrees (forward/up)
#define CLAWANGMAX 90
#define WRISTMIN 580  // 0 degrees (right)
#define WRISTMAX 2350 // 180 degrees (left)
#define WRISTANGMAX 180

Servo servoAlt, servoAzi;
Servo servoClaw, servoWrist;

void setAltitude(float angle)
{
  if(angle < 0) angle = 0;
  if(angle > ALTANGMAX) angle = ALTANGMAX;
  float us = ALTMIN + (angle*(ALTMAX-ALTMIN)/ALTANGMAX);
  servoAlt.writeMicroseconds(us);
}

void setAzimuth(float angle)
{
  if(angle < 0) angle = 0;
  if(angle > AZIANGMAX) angle = AZIANGMAX;
  float us = AZIMIN + (angle*(AZIMAX-AZIMIN)/AZIANGMAX);
  servoAzi.writeMicroseconds(us);
}

void setClaw(float angle)
{
  if(angle < 0) angle = 0;
  if(angle > CLAWANGMAX) angle = CLAWANGMAX;
  float us = CLAWMIN + (angle*(CLAWMAX-CLAWMIN)/CLAWANGMAX);
  servoClaw.writeMicroseconds(us);
}

void setWrist(float angle)
{
  if(angle < 0) angle = 0;
  if(angle > WRISTANGMAX) angle = WRISTANGMAX;
  float us = WRISTMIN + (angle*(WRISTMAX-WRISTMIN)/WRISTANGMAX);
  servoWrist.writeMicroseconds(us);
}

void setupCamera() {
  servoAlt.attach(SERVO_ALT_PIN);
  delay(100);
  setAltitude(90);
  servoAzi.attach(SERVO_AZI_PIN);
  delay(100);
  setAzimuth(90);
//  servoClaw.attach(SERVO_CLAW_PIN);
//  delay(100);
//  setClaw(0);
//  servoWrist.attach(SERVO_WRIST_PIN);
//  delay(100);
//  setWrist(90);
}

float t = 0;
void loopCamera() {
  float icosval4 = (0.5 - 0.5*cos(t*4*3.14159/180));
  float sinval = (0.5 + 0.5*sin(t*3.14159/180));
  t += 0.5;
//  setClaw(icosval4*CLAWANGMAX);
//  setWrist(sinval*WRISTANGMAX);
  setAzimuth(sinval*AZIANGMAX);
}
#endif

void setupRadio() {
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  //vw_set_rx_pin(RADIOPIN);
  vw_rx_start();
  Serial.println("START");
}

void loopRadio() {
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    Serial.print("Got: ");
    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

struct thread_t threads[] = {
//  {0, 50, setupCamera, loopCamera},
  {0, 1, setupRadio, loopRadio},
};
#define NUM_THREADS (sizeof(threads)/sizeof(struct thread_t))

void setup() {
  int i;
  Serial.begin(9600);
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i].lasttime = millis();
    threads[i].setupfunc();
  }
  Serial.println("START");
  digitalWrite(1, HIGH);
}

//void loop()
//{
//  int i;
//  unsigned long time = millis();
//  for (i = 0; i < NUM_THREADS; i++) {
//    if (time - threads[i].lasttime >= threads[i].timeslice) {
//      threads[i].loopfunc();
//      threads[i].lasttime = time;
//    }
//  }
//}


