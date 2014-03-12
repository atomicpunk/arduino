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
#include <Console.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

typedef void (*call_t)();
struct thread_t {
  unsigned long lasttime;
  unsigned long timeslice;
  call_t setupfunc;
  call_t loopfunc;
};

#include <Servo.h>
#define SERVO_ALT_PIN 11
#define SERVO_AZI_PIN 10
#define ALTMIN 700  // 0 degrees (down)
#define ALTMAX 2200 // 120 degrees (forward/up)
#define ALTANGMAX 140
#define ALTDEF 60
#define AZIMIN 880  // 0 degrees (right)
#define AZIMAX 2480 // 180 degrees (left)
#define AZIANGMAX 180
#define AZIDEF 80

Servo servoAlt, servoAzi;

float altitude = ALTDEF;
void setAltitude(float angle)
{
  servoAlt.attach(SERVO_ALT_PIN);
  delay(100);
  if(angle < 0) angle = 0;
  if(angle > ALTANGMAX) angle = ALTANGMAX;
  int us = ALTMIN + (angle*(ALTMAX-ALTMIN)/ALTANGMAX);
  altitude = angle;
  servoAlt.writeMicroseconds(us);
  delay(500);
  servoAlt.detach();
}

float azimuth = AZIDEF;
void setAzimuth(float angle)
{
  servoAzi.attach(SERVO_AZI_PIN);
  delay(100);
  if(angle < 0) angle = 0;
  if(angle > AZIANGMAX) angle = AZIANGMAX;
  int us = AZIMIN + (angle*(AZIMAX-AZIMIN)/AZIANGMAX);
  servoAzi.writeMicroseconds(us);
  azimuth = angle;
  delay(500);
  servoAzi.detach();
}

void runCommand(String rawcmd) {
  if(rawcmd == "l") {
    setAzimuth(azimuth+10);
  } else if(rawcmd == "r") {
    setAzimuth(azimuth-10);
  } else if(rawcmd == "u") {
    setAltitude(altitude+10);
  } else if(rawcmd == "d") {
    setAltitude(altitude-10);
  } else if(rawcmd == "l1") {
    setAzimuth(azimuth+1);
  } else if(rawcmd == "r1") {
    setAzimuth(azimuth-1);
  } else if(rawcmd == "u1") {
    setAltitude(altitude+1);
  } else if(rawcmd == "d1") {
    setAltitude(altitude-1);
  } else if((rawcmd == "c")||(rawcmd == "center")) {
    setAltitude(ALTDEF);
    setAzimuth(AZIDEF);
  } else if(rawcmd == "roof1") {
    setAltitude(30);
    setAzimuth(120);
  } else if(rawcmd == "roof2") {
    setAltitude(32);
    setAzimuth(24);
  } else if(rawcmd == "left") {
    setAltitude(60);
    setAzimuth(110);
  } else if(rawcmd == "right") {
    setAltitude(60);
    setAzimuth(63);
  } else if(rawcmd == "i") {
    Console.print("alt: ");
    Console.println(altitude);
    Console.print("azi: ");
    Console.println(azimuth);
  } else {
    String cmd = rawcmd.substring(0, 3);
    String arg = rawcmd.substring(3);
    int val = 0;
    if(cmd == "azi") {
      setAzimuth(arg.toInt());
    } else if(cmd == "alt") {
      setAltitude(arg.toInt());
    }
  }
}

YunServer server;
void setupServer() {
  server.listenOnLocalhost();
  server.begin();
}

void loopServer() {
  YunClient client = server.accept();

  if (client) {
    String command = client.readStringUntil('/');
    command.trim();
    runCommand(command);
    client.stop();
  }
}

struct thread_t threads[] = {
  {0, 20, setupServer, loopServer},
};
#define NUM_THREADS (sizeof(threads)/sizeof(struct thread_t))

void setup() {
  int i;
  setAltitude(ALTDEF);
  setAzimuth(AZIDEF);
  Bridge.begin();
  Console.begin();
  while (!Console) {
    ; // wait for Console port to connect.
  }
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i].lasttime = millis();
    threads[i].setupfunc();
  }
  Console.println("Yun1 Ready!");
}

String cmdString = "";
void loop()
{
  int i;
  unsigned long time = millis();
  for (i = 0; i < NUM_THREADS; i++) {
    if (time - threads[i].lasttime >= threads[i].timeslice) {
      threads[i].loopfunc();
      threads[i].lasttime = time;
    }
  }
  if (Console.available() > 0) {
    char c = Console.read();
    if((c == '=')||(c == '\n')) {
      runCommand(cmdString);
      cmdString = "";
    } else {
      cmdString += c;
    }
  }
}

