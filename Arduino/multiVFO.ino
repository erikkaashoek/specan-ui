/*
 * si5351_sweeper.ino - Si5351 Simple Sweep Generator
 *
 * Copyright (c) 2016 Thomas S. Knutsen <la3pna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Connect Si5351 to I2C
 * Sweep out is on pin 5, ranging from 0-5V (3.3V).
 * Use a filter on sweep out voltage. 100K + 1uF should be a good start.
 * A op-amp can be used to improve the filtering of the DC voltage.
 */

#include <Wire.h> 

#define LIB_SI5351_xxx
#ifdef LIB_SI5351
#include <si5351.h>
Si5351 si5351;
#else
#include <si5351.h>
Si5351 si5351;
#include "si5351_nano.h"
#endif

//#include "Wire.h"

//#define freqPin D6

volatile unsigned long cnt; // volatile var is stored in RAM (not register) for interrupt handler
void rpm() {
  cnt++;
}

long showFreq() {
  unsigned long cntl;
/*
  cli();
  cnt = 0;
  sei();
  // count for 1000 msec
  delay(1000);
  cli();
  cntl = cnt;
  sei();
*/
return (cntl);
}


int correction = 0; // use the Si5351 correction sketch to find the frequency correction factor

int inData = 0;
long steps = 100;
long  startFreq = 10000000;
long  stopFreq = 100000000;
long  lastFreq[3] = { 1000000, 1000000, 1000000 };
int VFO = 0;
int analogpin = A5;
int delaytime = 50;
int drive = 6;



int sensor;

/*
#include <MD_REncoder.h>
// set up encoder object
MD_REncoder R = MD_REncoder(2, 0);
#endif
*/

//const int buttonPin = D7;    // the number of the pushbutton pin
enum buttont_event {shortClickRelease=1, longClick=2, longClickRelease=3};
enum button_state {buttonUp, buttonDown, buttonLongDown};
int buttonState = buttonUp;             // the current reading from the input pin
int buttonEvent = 0;
//int lastButtonRead = HIGH;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long longPressDelay = 350;    // the longpress time; increase if the output flickers


long incrBase = 1000000;
int incrBaseDigit = 7 ; 

void showFreq(long f)
{
  int digit = 9;
  int leading = 1;
  long divider = 100000000;
  while (digit>0)
  {
    if (digit == 6) {
        Serial.print(".");
        leading = 0;
    }
    if (digit == incrBaseDigit)
        Serial.print("[");
    if (f / divider > 0)
      leading = 0;
//    if (!leading)
//    {
      Serial.print( (int) (f / divider) );
      f %= divider;      
//    }
//    else
//        Serial.print(" ");
    if (digit == incrBaseDigit)
        Serial.print("]");
    divider /= 10;
    digit -= 1;    
  }
  Serial.println(" ");
}


void info()
{
  Serial.println("Si5351 Sweeper");
  Serial.print("A = Start frequency, currently ");
  Serial.println(startFreq);
  Serial.print("B = Stop frequency, currently ");
  Serial.println(stopFreq);
  Serial.print("S = Steps, currently ");
  Serial.println(steps);
  Serial.println("M = Single sweep");
  Serial.println("C = Continious sweep until Q");
  Serial.println("H = show menu");
  Serial.print("T = Timestep in ms, currently ");
  Serial.println(delaytime);
  Serial.print("O = Output frequency, currently ");
  Serial.println(lastFreq[VFO]);
  Serial.print("D = Drive[2,4,6,8], currently ");
  Serial.println(drive);
  Serial.print("V = VFO[0,1,2], currently ");
  Serial.println(VFO);
  Serial.println("F = Measure frequency");
  showFreq(lastFreq[VFO]);
}



void setup() 
{
  Serial.begin(115200);

//  R.begin();
//  pinMode(buttonPin, INPUT_PULLUP);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  */
#ifdef LIB_SI5351  
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, correction);
  si5351.drive_strength((si5351_clock)VFO, SI5351_DRIVE_6MA);
#else
  si5351_init();
#endif
  //pinMode(freqPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(freqPin), rpm, RISING);

  info();
}

void histo(int lev)
{
  Serial.print(lev);
//  Serial.print(": ");
 // while (lev > 80) lev = lev / 2;  
//  while (lev--)
//    Serial.print("*");
  Serial.println("");
}

long old_time = 0;

void loop()
{
#if 0
  uint8_t x = R.read();
  int reading = 0;//digitalRead(buttonPin);
  if (reading != lastButtonRead) {
    lastDebounceTime = millis();
  }
  lastButtonRead = reading;
  
  buttonEvent = 0;
  if ((millis() - lastDebounceTime) > debounceDelay) {
    switch(buttonState) {
      case buttonUp: 
        if (reading == LOW) 
        {
          buttonState = buttonDown;
        }
       break;
      case buttonDown: 
        if (reading == HIGH)
        {
          buttonState = buttonUp;
          buttonEvent = shortClickRelease;
        }
        if ((millis() - lastDebounceTime) > longPressDelay) 
        {
          buttonState = buttonLongDown;
          buttonEvent = longClick;
        }
        break;
      case buttonLongDown: 
        if (reading == HIGH)
        {
          buttonState = buttonUp;
          buttonEvent = longClickRelease;
        }
        break;
        
    }
  }
  
  if (x) 
  {
    if (buttonState == buttonLongDown) 
    {
      if (x == DIR_CW) {
        if (incrBase > 10) {
          incrBase /= 10;
          incrBaseDigit--;
        }
      } else {
        if (incrBase < 100000000) {
          incrBase *= 10;
          incrBaseDigit++;
        }
      }
   }
    else {
      int incr;
      long new_time = millis();
      if (new_time - old_time > 50)
        incr = 1;
      else if (new_time - old_time > 20)
        incr = 5;
      else
        incr = 20;
      old_time = new_time;
      lastFreq[VFO] = (lastFreq[VFO] / incrBase) * incrBase; // round to incrBase
      if (x == DIR_CW) {
        lastFreq[VFO] += incr*incrBase;
        if (lastFreq[VFO] > 990000000)
          lastFreq[VFO] = 990000000;
      } else {
        lastFreq[VFO] -= incr*incrBase;
        if (lastFreq[VFO] < 0)
          lastFreq[VFO] = 0;
      }
      //Serial.println(lastFreq[VFO]);
    }
    showFreq(lastFreq[VFO]);
    
#ifdef LIB_SI5351  
    si5351.set_freq(lastFreq[VFO] * SI5351_FREQ_MULT, (si5351_clock)VFO);
#else
#endif
  }
#endif  
  inData = 0;
  if(Serial.available() > 0)   // see if incoming serial data:
  {
    inData = Serial.read();  // read oldest byte in serial buffer:
    Serial.println(inData);
  }
  if(inData == 'M' || inData == 'm')
  {
    unsigned long oldfreq, freq;
    inData = 0;
    unsigned long freqstep = (stopFreq - startFreq) / steps;
    unsigned long old_micros;
    freq = startFreq;
    for(int i = 0; i < (steps + 1); i++ )
    {
      old_micros = micros();
#ifdef LIB_SI5351  
       si5351.set_freq(freq * SI5351_FREQ_MULT, (si5351_clock)VFO);
#else
       si5351_set_frequency(VFO, freq, SI5351_CLK_DRIVE_STRENGTH_8MA);
#endif
      lastFreq[VFO] = freq;
      //analogWrite(analogpin, map(i, 0, steps, 0, 255));
      //delay(delaytime);
      if (i>0) {
#if 0
        Serial.println("r1:1");
#else
        Serial.print("r"); 
        Serial.print(oldfreq );
        Serial.print(":");
        Serial.println(sensor);
#endif
      }
      delaytime = 5;
      while (micros() - old_micros < delaytime * 100) {
        delayMicroseconds(100);
      }
      oldfreq = freq;
      freq = freq + freqstep;
      sensor = 1000 - analogRead(A0)*2;
    }
    Serial.print("r"); 
    Serial.print(freq );
    Serial.print(":");
    Serial.println(sensor);
    Serial.println("e");
    //si5351.output_enable(VFO, 0);
  }

  if(inData == 'C' || inData == 'c')
  {
    boolean running = true;
    inData = 0;
    while(running)
    {
      unsigned long freqstep = (stopFreq - startFreq) / steps;
      double f;
      for (int i = 0; i < (steps + 1); i++ )
      {
        unsigned long freq = startFreq + (freqstep * i);
#ifdef LIB_SI5351  
        si5351.set_freq(freq * SI5351_FREQ_MULT, (si5351_clock)VFO);
#else
        si5351_set_frequency(VFO, freq, SI5351_CLK_DRIVE_STRENGTH_8MA);
#endif
        lastFreq[VFO] = freq;
        analogWrite(analogpin, map(i, 0, steps, 0, 255));
        sensor = analogRead(A0);
        Serial.print(freq / 1000000.0);
        Serial.print(": ");
        histo(sensor);
        delay(delaytime);
        if(Serial.available() > 0)   // see if incoming serial data:
        {
          inData = Serial.read();  // read oldest byte in serial buffer:
          if(inData == 'Q' || inData == 'q')
          {
            running = false;
            inData = 0;
          }
        }
      }
      Serial.println(".");
    }
    //si5351.output_enable(VFO, 0);
  }

  if(inData == 'S' || inData == 's')
  {
    steps = Serial.parseInt();
    Serial.print("Steps: ");
    Serial.println(steps);
    inData = 0;
  }

  if(inData == 'H' || inData == 'h')
  {
    info();
  }

  if(inData == 'T' || inData == 't')
  {
    delaytime = Serial.parseInt();
    Serial.print("time pr step: ");
    Serial.println(delaytime);
    inData = 0;
  }

  if(inData == 'L' || inData == 'l')
  {
    for (int i = 0; i < (steps+1); i++ )
    {
      // print out the value you read:
      Serial.print(i * 10);
      Serial.print(';');
      Serial.print(steps);
      Serial.print(';');
      Serial.println(-i);
      delay(10);        // delay in between reads for stability
    }
    inData = 0;
  }

  if(inData == 'A' || inData == 'a')
  {
    startFreq = Serial.parseInt();
    Serial.print("Start: ");
    Serial.println(startFreq);
    inData = 0;
  }

  if(inData == 'B' || inData == 'b')
  {
    stopFreq = Serial.parseInt();
    Serial.print("Stop: ");
    Serial.println(stopFreq);
    inData = 0;
  }

  if(inData == 'O' || inData == 'o')
  {
    lastFreq[VFO] = Serial.parseInt();
    Serial.print("Output frequency: ");
    Serial.println(lastFreq[VFO]);
    si5351.set_freq(lastFreq[VFO] * SI5351_FREQ_MULT, (si5351_clock)VFO);
    inData = 0;
  }

  if(inData == 'D' || inData == 'd')
  {
    drive = Serial.parseInt();
    Serial.print("Drive: ");
    Serial.println(drive);
    si5351.drive_strength((si5351_clock)VFO, (si5351_drive) (((drive / 2)-1) & 3));
    inData = 0;
  }
  if(inData == 'V' || inData == 'v')
  {
    VFO = Serial.parseInt();
    Serial.print("VFO: ");
    Serial.println(VFO);
    inData = 0;
  }
  if(inData == 'F' || inData == 'f')
  {
    boolean running = true;
    inData = 0;
    while (running)
    {
      Serial.println(showFreq());
        if(Serial.available() > 0)   // see if incoming serial data:
        {
          inData = Serial.read();  // read oldest byte in serial buffer:
          if(inData == 'Q' || inData == 'q')
          {
            running = false;
            inData = 0;
          }
        }
    }
  }
}
