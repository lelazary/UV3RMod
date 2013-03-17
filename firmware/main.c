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


unsigned char	selfBias;		//
unsigned char	i;

//If we need the memory, we can start stuffing some bits together 
struct radioSettings
{
  short trFreqM; //The transmit mega portion of the freq (BCD)
  short trFreqK; //The transmit kilo portion of the freq (BCD)
  short rxFreqM; //The recive mega portion of the freq (BCD)
  short rxFreqK; //The recive kilo portion of the freq (BCD)

  unsigned char contrast;
  unsigned char power;
  unsigned char volume;
  unsigned char ctcss;
  unsigned char sqOpen;
  unsigned char sqClose;
  unsigned char txDTMF[4]; //8 diffrent dtmf transmitions
  unsigned char txFMDev;
  unsigned char options; //lpf,hpf,emp
};

int main()
{
  //Pin 31 is R10
  int i,j,k;

  initIOPorts();

  msDelay(100);
  getSelfBias();
  lcdInit(42); //Adjust this for LCD contrast


  unsigned char reg = 0;
  unsigned short b = '/';
  unsigned char pos = 0;

  LCD_BACKLIGHT = 1;
  lcdClear();
  lcdShowStr("HACKED",6);
  lcdShowStr("VER",0);
  lcdSetSymbol('.',0); //Lower period
  lcdShowStr("001",3);
  msDelay(2000); //Show startup screen for 2 seconds

  short encoderPos = 0;

  //Initial RDA settings
  //initRDA1846();

  lcdClear();

  while(1)
  {
    int k=0; 

    char encoderDir = getDialEncoder();
    unsigned char keys = getKeys();
    if (keys)
    {
      lcdShowNum(keys, 11, 16);
    }
    if (encoderDir)
    {
      encoderPos += encoderDir;
      lcdClear();
      lcdShowNum(encoderPos, 5, 10);
    }

    //unsigned char val = readADC(ADC_1); //Read the battery level
    WDTR	= 0x9F;
  }

  return 0;
}



