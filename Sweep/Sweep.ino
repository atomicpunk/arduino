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

#include <Servo.h> 

// servo and connection data
#define PIN 13
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
int delayval = T_SOFTMID;
bool sweep = false;
int sweepval = S_INC;
long sweepstop[3] = {10000, 0, 10000};

void setup() 
{ 
  Serial.begin(SERIAL_BAUD);
  myservo.attach(PIN);
  delay(1000);
  myservo.detach();
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

void loop() 
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
        Serial.print(pos);Serial.print(" -> ");Serial.println(newpos);
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
