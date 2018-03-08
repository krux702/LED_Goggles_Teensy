/*************************************************** 
LED Goggles Mk 3
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

#define LED_ON 2048  // 0 to 4095
#define FAST_FADE_VAL 128
#define FADE_VAL 64
#define SLOW_FADE_VAL 32
#define EFFECTS 20

#define RESET_STATE_ADDR 1020

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x42);
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x43);
Adafruit_PWMServoDriver pwm5 = Adafruit_PWMServoDriver(0x46);
Adafruit_PWMServoDriver pwm6 = Adafruit_PWMServoDriver(0x47);

#define NUM_LEDS 90
long DisplayBuffer[NUM_LEDS+27];
long AltDisplayBuffer[NUM_LEDS+27];
long tempBuffer[2];

uint8_t last_rand_num = 0;
uint8_t rand_num = 0;
uint8_t rdir = 0;

unsigned char LEDMode;

// MSGEQ7 spectrum analyser
// analog 1 is input
int strobe = 16; // strobe pins on analog 2
int res = 17; // reset pins on analog 3
int channel[7]; // store band values in these arrays
int band;


void setup() {
  Serial.begin(9600);

  // MSGEQ7 spectrum analyser
  pinMode(res, OUTPUT); // reset
  pinMode(strobe, OUTPUT); // strobe
  digitalWrite(res,LOW); // reset low
  digitalWrite(strobe,HIGH); //pin 5 is RESET on the shield

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!

  pwm1.begin();
  pwm1.setPWMFreq(1000);  // This is the maximum PWM frequency
  pwm1.setINV(1); // invert output

  pwm2.begin();
  pwm2.setPWMFreq(1000);
  pwm2.setINV(1);

  pwm3.begin();
  pwm3.setPWMFreq(1000);
  pwm3.setINV(1);

  pwm4.begin();
  pwm4.setPWMFreq(1000);
  pwm4.setINV(1);

  pwm5.begin();
  pwm5.setPWMFreq(1000);
  pwm5.setINV(1);

  pwm6.begin();
  pwm6.setPWMFreq(1000);
  pwm6.setINV(1);

  // save I2C bitrate
  uint8_t twbrbackup = TWBR;
  // must be changed after calling Wire.begin() (inside pwm1.begin())
  TWBR = 12; // 400KHz

  Serial.println("Turning all channels off");
  for (uint8_t pwmnum=0; pwmnum < 16; pwmnum++)
  {
    pwm1.setPWM(pwmnum, 0, 0 );
    pwm2.setPWM(pwmnum, 0, 0 );
    pwm3.setPWM(pwmnum, 0, 0 );
    pwm4.setPWM(pwmnum, 0, 0 );
    pwm5.setPWM(pwmnum, 0, 0 );
    pwm6.setPWM(pwmnum, 0, 0 );
  }

  // reset as a button!
  LEDMode = EEPROM.read(RESET_STATE_ADDR)%2;
  EEPROM.write(RESET_STATE_ADDR, (LEDMode + 1)%2);

  // display our current mode
  Serial.println(LEDMode);
  LongBarGraph(LEDMode+1, 0, 0, 128, 0, 0, 0);
  delay(1000);

  randomSeed(analogRead(0)+analogRead(5)+LEDMode);
}

void loop()
{
  long red1_val = 0;
  long green1_val = 0;
  long blue1_val = 0;
  long red2_val = 0;
  long green2_val = 0;
  long blue2_val = 0;

  while(rand_num == last_rand_num && rand_num < EFFECTS + 1)
  {
    rand_num = random(1, EFFECTS + 1);
  }
  last_rand_num = rand_num;

  if(LEDMode == 1)
  {
    //    rand_num=20;
  }
  
  switch(rand_num) {
    case 1:
      RandomThreeColor(1792,4095,0,LED_ON,0,LED_ON,0,LED_ON,LED_ON);
      FadeOut();
      break;
    case 2:
      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 8 ; led_num++)
        {
          BarGraph(led_num, LED_ON,0,LED_ON, 0,0,0);
          delay(125);
        }
      }
      FadeOut();
      break;
    case 3:
      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 8 ; led_num++)
        {
          BarGraph(led_num, 0,LED_ON,LED_ON, LED_ON,0,LED_ON);
          delay(125);
        }
      }
      FadeOut();
      break;
    case 4:
      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 8 ; led_num++)
        {
          BarGraph(led_num, 0,0,LED_ON, 0,LED_ON,LED_ON);
          delay(125);
        }
      }
      FadeOut();
      break;
    case 5:
      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 8 ; led_num++)
        {
          BarGraph(led_num, 0,0,0, 0,0,LED_ON);
          delay(125);
        }
      }
      FadeOut();
      break;
    case 6:
      // purple and yellow mode
      RandomTwoColor(1792,4095,0,LED_ON,0,LED_ON);
      FadeOut();
      break;
    case 7:
      // random two color mode
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      RandomTwoColor(red1_val,green1_val,blue1_val,red2_val,green2_val,blue2_val);

      FadeOut();
      break;
    case 8:      
     CycleLEDs(); 
      FadeOut();
      break;
    case 9:  
      Sparkle(256);
      FadeOut();
      break;
    case 10:
      ColorChaser();
      FadeOut();
      break;
    case 11:
      RandomWhite();
      FadeOut();
      break;
    case 12:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 8 ; led_num++)
        {
          BarGraph(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
        for(uint8_t led_num = 8 ; led_num > 0 ; led_num--)
        {
          BarGraph(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 13:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 16 ; led_num++)
        {
          LongBarGraph(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
        for(uint8_t led_num = 16 ; led_num > 0 ; led_num--)
        {
          LongBarGraph(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 14:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      LongBarGraph(8, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 16 ; led_num++)
        {
          Rotate(1);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 15:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      LongBarGraph(8, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 16 ; led_num++)
        {
          Rotate(0);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 16:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num < 8 ; led_num++)
        {
          SmallWipe(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
        for(uint8_t led_num = 8 ; led_num > 0 ; led_num--)
        {
          SmallWipe(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 17:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      for(uint8_t loops = 0; loops < 3 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num < 16 ; led_num++)
        {
          BigWipe(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
        for(uint8_t led_num = 16 ; led_num > 0 ; led_num--)
        {
          BigWipe(led_num, red2_val, green2_val, blue2_val, red1_val, green1_val, blue1_val);
          delay(125);
        }
      }
      FadeOut();

      break;
    case 18:
      red1_val = RandomColor(128);
      green1_val = RandomColor(128);
      blue1_val = RandomColor(128);
      red2_val = RandomColor(128);
      green2_val = RandomColor(128);
      blue2_val = RandomColor(128);

      RandomTwoColorFill(red1_val,green1_val,blue1_val,red2_val,green2_val,blue2_val);

      if(random(255) < 128)
      {
        rdir = 1;
      }
      else
      {
        rdir = 0;
      } 

      for(uint8_t loops = 0; loops < 4 ; loops++)
      {
        for(uint8_t led_num = 0 ; led_num <= 16 ; led_num++)
        {
          Rotate(rdir);
          delay(90);
        }
      }
      FadeOut();

      break;
    case 20:
      for(int i = 0 ; i < 20 ; i++)
      {
        red1_val = RandomColor(112);
        green1_val = RandomColor(112);
        blue1_val = RandomColor(112);
        red2_val = RandomColor(112);
        green2_val = RandomColor(112);
        blue2_val = RandomColor(112);

        for(uint8_t loops = 0; loops < 3 ; loops++)
        {
           RandomTwoColorFill(red1_val,green1_val,blue1_val,red2_val,green2_val,blue2_val);
        }
        FadeOut();
      }

      break;
  }
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(res, HIGH);
  digitalWrite(res, LOW);
  for(band=0; band <7; band++)
  {
    digitalWrite(strobe,LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    channel[band] = analogRead(1); // store channel band reading
    digitalWrite(strobe,HIGH); 
  }
}

void displayBuffer(void)
{
  for (uint8_t i = 0 ; i < NUM_LEDS ; i++)
  {
    if(i < 15)
    {
      pwm1.setPWM(i, 0, DisplayBuffer[i]);
    }
    else if(i < 30)
    {
      pwm2.setPWM(i - 15, 0, DisplayBuffer[i]);
    }
    else if(i < 45)
    {
      pwm3.setPWM(i - 30, 0, DisplayBuffer[i]);
    }
    else if(i < 60)
    {
      pwm4.setPWM(i - 45, 0, DisplayBuffer[i]);
    }
    else if(i < 75)
    {
      pwm5.setPWM(i - 60, 0, DisplayBuffer[i]);
    }
    else
    {
      pwm6.setPWM(i - 75, 0, DisplayBuffer[i]);
    }
    
  } 
}

void fadeBuffer(int fade_val)
{
  for (uint8_t i = 0 ; i < NUM_LEDS ; i++)
  {
    if(DisplayBuffer[i] > fade_val)
    {
      DisplayBuffer[i] = DisplayBuffer[i] - fade_val;
    }
    else if(DisplayBuffer[i] > 0)
    {
      DisplayBuffer[i] = 0;
      AltDisplayBuffer[i] = 0;
    }
  }
}

void fadeFrame(void)
{
  for (uint8_t i = 0 ; i < NUM_LEDS ; i++)
  {
    if(DisplayBuffer[i] > AltDisplayBuffer[i])
    {
      DisplayBuffer[i] = DisplayBuffer[i] - AltDisplayBuffer[i];
    }
    else if(DisplayBuffer[i] > 0)
    {
      DisplayBuffer[i] = 0;
    }
  }
}

void RandomTwoColor(long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  for (long loop = 0; loop < NUM_LEDS * 4; loop++)
  { 
    uint8_t pwmnum = random(NUM_LEDS/3)*3;
    if(random(255) < 128)
    {
      DisplayBuffer[pwmnum] = blue1;
      DisplayBuffer[pwmnum+1] = green1;
      DisplayBuffer[pwmnum+2] = red1;
      AltDisplayBuffer[pwmnum] = blue1/16;
      AltDisplayBuffer[pwmnum+1] = green1/16;
      AltDisplayBuffer[pwmnum+2] = red1/16;
    }
    else
    {
      DisplayBuffer[pwmnum] = blue2;
      DisplayBuffer[pwmnum+1] = green2;
      DisplayBuffer[pwmnum+2] = red2;
      AltDisplayBuffer[pwmnum] = blue2/16;
      AltDisplayBuffer[pwmnum+1] = green2/16;
      AltDisplayBuffer[pwmnum+2] = red2/16;
    }

    displayBuffer();
    fadeFrame();
    delay(20);
  }
}

void RandomTwoColorFill(long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  for (long loop = 0; loop < NUM_LEDS ; loop++)
  { 
    uint8_t pwmnum = random(NUM_LEDS/3)*3;
    if(random(255) < 128)
    {
      DisplayBuffer[pwmnum] = blue1;
      DisplayBuffer[pwmnum+1] = green1;
      DisplayBuffer[pwmnum+2] = red1;
      AltDisplayBuffer[pwmnum] = blue1/16;
      AltDisplayBuffer[pwmnum+1] = green1/16;
      AltDisplayBuffer[pwmnum+2] = red1/16;
    }
    else
    {
      DisplayBuffer[pwmnum] = blue2;
      DisplayBuffer[pwmnum+1] = green2;
      DisplayBuffer[pwmnum+2] = red2;
      AltDisplayBuffer[pwmnum] = blue2/16;
      AltDisplayBuffer[pwmnum+1] = green2/16;
      AltDisplayBuffer[pwmnum+2] = red2/16;
    }

    fadeFrame();
  }
}

void RandomThreeColor(long red1, long green1, long blue1, long red2, long green2, long blue2, long red3, long green3, long blue3)
{
  for (long loop = 0; loop < NUM_LEDS * 4; loop++)
  { 
    uint8_t pwmnum = random(NUM_LEDS/3)*3;
    if(random(255) < 64)
    {
      DisplayBuffer[pwmnum] = blue1;
      DisplayBuffer[pwmnum+1] = green1;
      DisplayBuffer[pwmnum+2] = red1;
      AltDisplayBuffer[pwmnum] = blue1/16;
      AltDisplayBuffer[pwmnum+1] = green1/16;
      AltDisplayBuffer[pwmnum+2] = red1/16;
    }
    else if(random(255) < 128)
    {
      DisplayBuffer[pwmnum] = blue2;
      DisplayBuffer[pwmnum+1] = green2;
      DisplayBuffer[pwmnum+2] = red2;
      AltDisplayBuffer[pwmnum] = blue2/16;
      AltDisplayBuffer[pwmnum+1] = green2/16;
      AltDisplayBuffer[pwmnum+2] = red2/16;
    }
    else
    {
      DisplayBuffer[pwmnum] = blue3;
      DisplayBuffer[pwmnum+1] = green3;
      DisplayBuffer[pwmnum+2] = red3;
      AltDisplayBuffer[pwmnum] = blue3/16;
      AltDisplayBuffer[pwmnum+1] = green3/16;
      AltDisplayBuffer[pwmnum+2] = red3/16;
    }

    displayBuffer();
    fadeFrame();
    delay(20);
  }
}

void RandomWhite(void)
{
  // random
  for (long loop = 0; loop < NUM_LEDS * 4; loop++)
  { 
    uint8_t pwmnum = random(NUM_LEDS/3)*3;
    long pwmval = random(512);
    DisplayBuffer[pwmnum] = pwmval;
    DisplayBuffer[pwmnum+1] = pwmval;
    DisplayBuffer[pwmnum+2] = pwmval;
    fadeBuffer(FADE_VAL);
    displayBuffer();
    delay(20);
  }
}

void Sparkle(long num_loops)
{
  long red_val = 0;
  long green_val = 0;
  long blue_val = 0;
     // random
  for (long loop = 0; loop < num_loops; loop++)
  { 
    uint8_t pwmnum = random(NUM_LEDS/3);
    red_val = RandomColor(128);
    green_val = RandomColor(128);
    blue_val = RandomColor(128);

    DisplayBuffer[pwmnum*3] = blue_val;
    DisplayBuffer[pwmnum*3+1] = green_val;
    DisplayBuffer[pwmnum*3+2] = red_val;
    AltDisplayBuffer[pwmnum*3] = blue_val/16;
    AltDisplayBuffer[pwmnum*3+1] = green_val/16;
    AltDisplayBuffer[pwmnum*3+2] = red_val/16;
    displayBuffer();

    fadeFrame();

    delay(20);
  }
}

void FadeOut(void)
{
  // fade out
  for (uint8_t loops=0; loops < 16; loops++)
  {
    fadeBuffer(FAST_FADE_VAL);
    displayBuffer();
    delay(20);
  }
}

void ColorChaser(void)
{
  // color chaser
  for (uint8_t adder=0; adder < 3; adder++)
  {
    for (uint8_t pwmnum=adder; pwmnum <= NUM_LEDS + 24 - adder; pwmnum = pwmnum + 3)
    {
      if(pwmnum - 24 >=0)
      {
        DisplayBuffer[pwmnum-24] = 0;
      }
      if(pwmnum - 21 >=0)
      {
        DisplayBuffer[pwmnum-21] = 32;
      }
      if(pwmnum - 18 >=0)
      {
        DisplayBuffer[pwmnum-18] = 78;
      }
      if(pwmnum - 15 >=0)
      {
        DisplayBuffer[pwmnum-15] = 133;
      }
      if(pwmnum - 12 >=0)
      {
        DisplayBuffer[pwmnum-12] = 256;
      }
      if(pwmnum - 9 >=0)
      {
        DisplayBuffer[pwmnum-9] = 133;
      }
      if(pwmnum - 6 >=0)
      {
        DisplayBuffer[pwmnum-6] = 78;
      }
      if(pwmnum - 3 >=0)
      {
        DisplayBuffer[pwmnum-3] = 32;
      }
      displayBuffer();
      delay(100);
    }
  }  
}

void CycleLEDs()
{
  // cycle all LEDs through each color
  for (uint8_t color_loops=0; color_loops < 2; color_loops++)
  {
    for (uint8_t adder=0; adder < 3; adder++)
    {
      for (uint8_t loops=0; loops < 32; loops++)
      {
        for (uint8_t pwmnum=adder; pwmnum <= NUM_LEDS; pwmnum = pwmnum + 3)
        {
          DisplayBuffer[pwmnum] = DisplayBuffer[pwmnum] + FADE_VAL;
        }
        fadeBuffer(SLOW_FADE_VAL);
        displayBuffer();
        delay(20);
      }
      for (uint8_t loops=0; loops < 16; loops++)
      {
        fadeBuffer(SLOW_FADE_VAL);
        displayBuffer();
        delay(20);
      }
    }
  }
}

long RandomColor(uint8_t seed)
{
  if(random(255) < seed)
  {
    return random(LED_ON)+16;
  }
  else
  {
    return 0;
  }
}

void BarGraph(uint8_t lednum,long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  /*  1 - 8 , 15 - 8 
      16 - 23 , 30 - 23
                         */
  for(uint8_t i = 0 ; i < 8 ; i++)
  {
    if(i< lednum)
    {
      DisplayBuffer[i*3] = blue1;
      DisplayBuffer[i*3+1] = green1;
      DisplayBuffer[i*3+2] = red1;
      DisplayBuffer[(14-i)*3] = blue1;
      DisplayBuffer[(14-i)*3+1] = green1;
      DisplayBuffer[(14-i)*3+2] = red1;
      DisplayBuffer[(15+i)*3] = blue1;
      DisplayBuffer[(15+i)*3+1] = green1;
      DisplayBuffer[(15+i)*3+2] = red1;
      DisplayBuffer[(29-i)*3] = blue1;
      DisplayBuffer[(29-i)*3+1] = green1;
      DisplayBuffer[(29-i)*3+2] = red1;
    }
    else
    {
      DisplayBuffer[i*3] = blue2;
      DisplayBuffer[i*3+1] = green2;
      DisplayBuffer[i*3+2] = red2;
      DisplayBuffer[(14-i)*3] = blue2;
      DisplayBuffer[(14-i)*3+1] = green2;
      DisplayBuffer[(14-i)*3+2] = red2;
      DisplayBuffer[(15+i)*3] = blue2;
      DisplayBuffer[(15+i)*3+1] = green2;
      DisplayBuffer[(15+i)*3+2] = red2;
      DisplayBuffer[(29-i)*3] = blue2;
      DisplayBuffer[(29-i)*3+1] = green2;
      DisplayBuffer[(29-i)*3+2] = red2;
    }
  }  
  displayBuffer();
}

void LongBarGraph(uint8_t lednum,long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  /*  1 - 8 , 15 - 8 
      16 - 23 , 30 - 23
                         */
  for(uint8_t i = 0 ; i < 16 ; i++)
  {
    if(i< lednum)
    {
      DisplayBuffer[i*3] = blue1;
      DisplayBuffer[i*3+1] = green1;
      DisplayBuffer[i*3+2] = red1;
      DisplayBuffer[(29-i)*3] = blue1;
      DisplayBuffer[(29-i)*3+1] = green1;
      DisplayBuffer[(29-i)*3+2] = red1;
    }
    else
    {
      DisplayBuffer[i*3] = blue2;
      DisplayBuffer[i*3+1] = green2;
      DisplayBuffer[i*3+2] = red2;
      DisplayBuffer[(29-i)*3] = blue2;
      DisplayBuffer[(29-i)*3+1] = green2;
      DisplayBuffer[(29-i)*3+2] = red2;
    }
  }  
  displayBuffer();
}

void Rotate(uint8_t direction)
{
  /*  1 - 8 , 15 - 8 
      16 - 23 , 30 - 23
                         */
  long red1_val = 0;
  long green1_val = 0;
  long blue1_val = 0;
  long red2_val = 0;
  long green2_val = 0;
  long blue2_val = 0;
                    
  if(direction)
  {
    red1_val = DisplayBuffer[(0*3)];
    green1_val = DisplayBuffer[(0*3)+1];
    blue1_val = DisplayBuffer[(0*3)+2];
    red2_val = DisplayBuffer[(15*3)];
    green2_val = DisplayBuffer[(15*3)+1];
    blue2_val = DisplayBuffer[(15*3)+2];
    for(uint8_t i = 0 ; i < 14 ; i++)
    {
      DisplayBuffer[i*3] = DisplayBuffer[(i+1)*3];
      DisplayBuffer[i*3+1] = DisplayBuffer[(i+1)*3+1];
      DisplayBuffer[i*3+2] = DisplayBuffer[(i+1)*3+1];
      DisplayBuffer[(15+i)*3] = DisplayBuffer[(i+16)*3];
      DisplayBuffer[(15+i)*3+1] = DisplayBuffer[(i+16)*3+1];
      DisplayBuffer[(15+i)*3+2] = DisplayBuffer[(i+16)*3+1];
    }
    DisplayBuffer[(14*3)] = red1_val;
    DisplayBuffer[(14*3)+1] = green1_val;
    DisplayBuffer[(14*3)+2] = blue1_val;
    DisplayBuffer[(29*3)] = red2_val;
    DisplayBuffer[(29*3)+1] = green2_val;
    DisplayBuffer[(29*3)+2] = blue2_val;
  }
  else
  {
    red1_val = DisplayBuffer[(14*3)];
    green1_val = DisplayBuffer[(14*3)+1];
    blue1_val = DisplayBuffer[(14*3)+2];
    red2_val = DisplayBuffer[(29*3)];
    green2_val = DisplayBuffer[(29*3)+1];
    blue2_val = DisplayBuffer[(29*3)+2];
    for(uint8_t i = 14 ; i > 0 ; i--)
    {
      DisplayBuffer[i*3] = DisplayBuffer[(i-1)*3];
      DisplayBuffer[i*3+1] = DisplayBuffer[(i-1)*3+1];
      DisplayBuffer[i*3+2] = DisplayBuffer[(i-1)*3+1];
      DisplayBuffer[(15+i)*3] = DisplayBuffer[(i+14)*3];
      DisplayBuffer[(15+i)*3+1] = DisplayBuffer[(i+14)*3+1];
      DisplayBuffer[(15+i)*3+2] = DisplayBuffer[(i+14)*3+1];
    }
    DisplayBuffer[(0*3)] = red1_val;
    DisplayBuffer[(0*3)+1] = green1_val;
    DisplayBuffer[(0*3)+2] = blue1_val;
    DisplayBuffer[(15*3)] = red2_val;
    DisplayBuffer[(15*3)+1] = green2_val;
    DisplayBuffer[(15*3)+2] = blue2_val;
  }
  displayBuffer();
}


void SmallWipe(uint8_t lednum,long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  /*  
     3,2,1,0,14,13,12,11
     4,5,6,7,8 ,9 ,10,11
     18,17,16,15,29,28,27,26
     19,20,21,22,23,24,25,26
   */
  
  uint8_t LedSequence[32] = {11,12,13,14,0 ,1 ,2 ,3 ,
                             10,9 ,8 ,7 ,6 ,5 ,4 ,3 ,
                             26,27,28,29,15,16,17,18,
                             25,24,23,22,21,20,19,18};

  for(uint8_t i = 0 ; i < 8 ; i++)
  {
    if(i< lednum)
    {
      DisplayBuffer[(LedSequence[i])*3] = red1;
      DisplayBuffer[(LedSequence[i])*3+1] = green1;
      DisplayBuffer[(LedSequence[i])*3+2] = blue1;
      DisplayBuffer[(LedSequence[i+8])*3] = red1;
      DisplayBuffer[(LedSequence[i+8])*3+1] = green1;
      DisplayBuffer[(LedSequence[i+8])*3+2] = blue1;
      DisplayBuffer[(LedSequence[i+16])*3] = red1;
      DisplayBuffer[(LedSequence[i+16])*3+1] = green1;
      DisplayBuffer[(LedSequence[i+16])*3+2] = blue1;
      DisplayBuffer[(LedSequence[i+24])*3] = red1;
      DisplayBuffer[(LedSequence[i+24])*3+1] = green1;
      DisplayBuffer[(LedSequence[i+24])*3+2] = blue1;
    }
    else
    {
      DisplayBuffer[(LedSequence[i])*3] = red2;
      DisplayBuffer[(LedSequence[i])*3+1] = green2;
      DisplayBuffer[(LedSequence[i])*3+2] = blue2;
      DisplayBuffer[(LedSequence[i+8])*3] = red2;
      DisplayBuffer[(LedSequence[i+8])*3+1] = green2;
      DisplayBuffer[(LedSequence[i+8])*3+2] = blue2;
      DisplayBuffer[(LedSequence[i+16])*3] = red2;
      DisplayBuffer[(LedSequence[i+16])*3+1] = green2;
      DisplayBuffer[(LedSequence[i+16])*3+2] = blue2;
      DisplayBuffer[(LedSequence[i+24])*3] = red2;
      DisplayBuffer[(LedSequence[i+24])*3+1] = green2;
      DisplayBuffer[(LedSequence[i+24])*3+2] = blue2;
    }
  }   

  displayBuffer();

}

void BigWipe(uint8_t lednum,long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  /*  
     3,2,1,0,14,13,12,11
     4,5,6,7,8 ,9 ,10,11
     18,17,16,15,29,28,27,26
     19,20,21,22,23,24,25,26
   */
  
  uint8_t LedSequence[32] = {11,12,13,14,0 ,1 ,2 ,3 ,
                             26,27,28,29,15,16,17,18,
                             10,9 ,8 ,7 ,6 ,5 ,4 ,3 ,
                             25,24,23,22,21,20,19,18};

  for(uint8_t i = 0 ; i < 16 ; i++)
  {
    if(i< lednum)
    {
      DisplayBuffer[(LedSequence[i])*3] = red1;
      DisplayBuffer[(LedSequence[i])*3+1] = green1;
      DisplayBuffer[(LedSequence[i])*3+2] = blue1;
      DisplayBuffer[(LedSequence[i+16])*3] = red1;
      DisplayBuffer[(LedSequence[i+16])*3+1] = green1;
      DisplayBuffer[(LedSequence[i+16])*3+2] = blue1;
    }
    else
    {
      DisplayBuffer[(LedSequence[i])*3] = red2;
      DisplayBuffer[(LedSequence[i])*3+1] = green2;
      DisplayBuffer[(LedSequence[i])*3+2] = blue2;
      DisplayBuffer[(LedSequence[i+16])*3] = red2;
      DisplayBuffer[(LedSequence[i+16])*3+1] = green2;
      DisplayBuffer[(LedSequence[i+16])*3+2] = blue2;
    }
  }   

  displayBuffer();

}


void BigWipeFade(uint8_t lednum,long red1, long green1, long blue1, long red2, long green2, long blue2)
{
  /*  
     3,2,1,0,14,13,12,11
     4,5,6,7,8 ,9 ,10,11
     18,17,16,15,29,28,27,26
     19,20,21,22,23,24,25,26
   */

  uint8_t LedSequence[32] = {11,12,13,14,0 ,1 ,2 ,3 ,
                             26,27,28,29,15,16,17,18,
                             10,9 ,8 ,7 ,6 ,5 ,4 ,3 ,
                             25,24,23,22,21,20,19,18};

  for(uint8_t i = 0 ; i < 16 ; i++)
  {
    uint8_t percent = (lednum - i + 16)*6;

    if(percent < 0)
    {
      percent = 0;
    }
    if(percent > 100)
    {
      percent = 100;
    }
    
    DisplayBuffer[(LedSequence[i])*3] = MixColors(red1, red2, percent);
    DisplayBuffer[(LedSequence[i])*3+1] = MixColors(green1, green2, percent);;
    DisplayBuffer[(LedSequence[i])*3+2] = MixColors(blue1, blue2, percent);
    DisplayBuffer[(LedSequence[16+i])*3] = MixColors(red1, red2, percent);
    DisplayBuffer[(LedSequence[16+i])*3+1] = MixColors(green1, green2, percent);;
    DisplayBuffer[(LedSequence[16+i])*3+2] = MixColors(blue1, blue2, percent);
  }

  displayBuffer();

}

long MixColors(long color1, long color2, uint8_t percent)
{
  long mixed_color = ((color1 * percent) + (color2 * (100 - percent)))/100;
  return mixed_color;
}


