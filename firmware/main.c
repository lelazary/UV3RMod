/* 
 * This file is part of the uv3r firmware
 * More info at www.liorelazary.com
 * 
 * Created by Lior Elazary (KK6BWA) Copyright (C) 2013 <lior at elazary dot com> 
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA 
 */ 

#include <MC81F8816/MC81F8816.h>
#include <hms800.h>
#include "lcd.h"
#include "time.h"
#include "uv3r.h"

#define TRUE 1
#define FALSE 0

unsigned char selfBias;
unsigned char	i;

struct RadioSettings
{
  short rxFreqM; //The receive mega portion of the freq (BCD)
  short rxFreqK; //The receive kilo portion of the freq (BCD)
  short txFreqM; //The transmit mega portion of the freq (BCD)
  short txFreqK; //The transmit kilo portion of the freq (BCD)
  char offset;
  //unsigned char contrast;
  //unsigned char power;
  //unsigned char volume;
  //unsigned char ctcss;
  //unsigned char sqOpen;
  //unsigned char sqClose;
  //unsigned char txDTMF[4]; //8 different dtmf transmissions
  //unsigned char txFMDev;
  //unsigned char options; //lpf,hpf,emp

  unsigned char transmitting;
};

struct RadioSettings radioSettings;


void initRadioSettings()
{
  radioSettings.rxFreqM = 145;
  radioSettings.rxFreqK = 525;

  radioSettings.txFreqM = 145;
  radioSettings.txFreqK = 525;

  radioSettings.offset = 0;
  
  radioSettings.transmitting = FALSE;

}

void showFreqDisplayMode(unsigned char showTX) 
{
  lcdClear();
  if (showTX)
  {
    lcdShowNum(radioSettings.txFreqM, 8, 10);
    lcdShowNum(radioSettings.txFreqK, 11, 10);
  } else {
    lcdShowNum(radioSettings.rxFreqM, 8, 10);
    lcdShowNum(radioSettings.rxFreqK, 11, 10);
  }

  lcdSmallNumber(radioSettings.offset);
  lcdShowStr("1273PL",0);

}

enum DISPLAY_MODE {FREQ_DISPLAY};
void updateDisplay(unsigned char mode)
{
  switch(mode)
  {
    case FREQ_DISPLAY:
      showFreqDisplayMode(radioSettings.transmitting);
      break;
  }
}

void updateNum(unsigned short* num, unsigned char digit, char encDir)
{
  switch(digit)
  {
    case 0:
      if (encDir > 0)
        *num += 100;
      else
        *num -= 100;
      break;
    case 1:
      if (encDir > 0)
        *num += 10;
      else
        *num -= 10;
      break;
    case 2:
      *num += encDir;
      break;
  }

  if (*num > 999) 
    *num = 0;
}


unsigned char changeMode = 0;
int main()
{
  //Pin 31 is R10
  initIOPorts();

  msDelay(100);
  getSelfBias();
  lcdInit(42); //Adjust this for LCD contrast

  initRadioSettings();

  LCD_BACKLIGHT = 0;
  lcdClear();
  lcdShowStr("HACKED",6);
  lcdShowStr("VER",0);
  //lcdSetSymbol('.',0); //Lower period
  lcdShowStr("001",3);
  msDelay(1000); //Show startup screen for 2 seconds

  short encoderPos = 0;

  //Initial RDA settings
  rda1846Init();

  lcdClear();

  updateDisplay(FREQ_DISPLAY);
  while(1)
  {
    int k=0; 

    unsigned char keys = getKeys();
    if (keys)
    {
      switch(keys)
      {
        case VOL_KEY:
          changeMode++;
          if (changeMode > 6)
            changeMode = 0;
          break;
        case PTT_KEY:
          radioSettings.transmitting = !radioSettings.transmitting;
          if (radioSettings.transmitting)
          {
            LCD_BACKLIGHT = 1;
            rda1846TX();
          }
          else
          {
            rda1846RX(1);
            LCD_BACKLIGHT = 0;
          }
          break;
      }
      updateDisplay(FREQ_DISPLAY);
    } else {
      //radioSettings.transmitting = 0;
    }

    char encoderDir = getDialEncoder();
    if (encoderDir)
    {
      if (!radioSettings.transmitting) //Dont change while transmitting
      {
        if (changeMode > 5)
          radioSettings.offset += encoderDir;
        if (changeMode > 2)
          updateNum(&radioSettings.rxFreqK, changeMode-3, encoderDir);
        else
          updateNum(&radioSettings.rxFreqM, changeMode, encoderDir);

      }

      updateDisplay(FREQ_DISPLAY);
    }

    radioSettings.txFreqM = radioSettings.rxFreqM + radioSettings.offset;
    radioSettings.txFreqK = radioSettings.rxFreqK; 

    //unsigned char val = readADC(ADC_1); //Read the battery level
    WDTR	= 0x9F;
  }

  return 0;
}



