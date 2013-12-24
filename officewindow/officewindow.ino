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

//#define USE_SERVO 1
#include <Arduino.h>

#define RADIOPIN 1

#ifdef USE_SERVO
#include <Servo.h> 

// servo and connection data
#define SERVOPIN 11
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

Servo myservo;
String cmdString = "";
int pos=A_RESET, newpos=A_RESET;
int delayval = 1;

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
      newpos = temp;
    }
    else
    {
      Serial.print(temp);
      Serial.println("deg out of range");
    }
    break;
  case 'i':
    Serial.print("Angle: ");      
    Serial.println(pos);
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
#endif

void loop2() 
{
#ifdef USE_SERVO
  checkForCommand();

  if (newpos != pos) {
    if(!myservo.attached())
    {
      myservo.attach(SERVOPIN);
    }
    int newval = a_to_ms(newpos);
    int val = a_to_ms(pos);
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
    myservo.detach();
  }
#endif
}

#define BOARD_UNO 1
#include "/home/tebrandt/workspace/arduino/ledstrip/ledstrip.h"

void setup() {
  setupLedStrip();
#ifdef USE_SERVO
  cmdString = "";
  pos=A_RESET;
  newpos=35;
  Serial.begin(SERIAL_BAUD);
  myservo.attach(SERVOPIN);
  delay(1000);
  myservo.detach();
#endif
}

void loop()
{
  int i;

  for (i=0;i<pattern_size;i++)
  {
    send_pattern_line(pattern[i]);
    delay(100);
  }
}

