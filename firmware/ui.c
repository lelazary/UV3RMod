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
#include "uv3r.h"
#include "uart.h"
#include "ui.h"

unsigned char changeMode = 0;
unsigned char displayMode = FREQ_DISPLAY;


void showStatusDisplayMode()
{
  lcdClear();
  lcdSetFlashPos(0xff);

  lcdShowStr("VS",6);
  lcdShowNum(radioSettings.vssi, 11, 10);

  lcdShowStr("RS",0);
  lcdShowNum(radioSettings.rssi, 5, 10);

}

void showDTMFDisplayMode()
{
  lcdClear();
  lcdShowStr("DTMFTX",6);

  unsigned i;
  for(i=0; i<6; i++)
  {
    unsigned char digi = radioSettings.txDTMF[i];
    if (digi < 0x10)
    {
      digi = digi < 10 ? '0' + digi : 'A' + digi - 10;
      lcdAlphaNum(i, digi);
    } else {
      lcdAlphaNum(i, ' ');
    }

  }
}

void updateDisplay(unsigned char mode)
{

  //if (!(updateTime++%100)) //Update the display every 100 loops so it will not fliker
  {
    switch(mode)
    {
      case STATUS_DISPLAY:
        showStatusDisplayMode();
        break;
      case DTMF_DISPLAY:
        showDTMFDisplayMode();
        break;
    }
  }
}

void updateNum(unsigned short* num, unsigned char digit, char encDir)
{
  unsigned short val = *num;
  switch(digit)
  {
    case 0:
      if (encDir > 0)
        val += 100;
      else
        val -= 100;
      break;
    case 1:
      if (encDir > 0)
        val += 10;
      else
        val -= 10;
      break;
    case 2:
      val += encDir;
      break;
  }

  if (val <= 999) 
    *num = val;
}

void updateRDA1846Freq(unsigned short freqM, unsigned short freqK)
{
  if (freqM > 500)
    return;

  //Convert from DCB to binary of freqM and freqK while using short (2 byte) numbers
  //The rda1846 keeps the freq in 1khz/8 steps
  //so: 32Bit val = freqM*8000 + freqK*8
  //since 8000 can be broken down into 125*64 we can first 

  unsigned short freqKs = freqK;

  //Since freqK is a fraction of 1000, 
  //convert it into a whole number
  if (freqK > 0 && freqK < 10)
    freqKs = freqK*100;
  else if (freqK >= 10 && freqK < 100)
    freqKs = freqK*10;

  unsigned short tmp = freqM*125;
  unsigned short highWord = (tmp >> 10); //Get the top 4 bits
  tmp = (tmp << 6); //Just get the lower bits

  unsigned short lowWord =  tmp + (freqK<<3); //tmp + freqK*8
  if (lowWord < tmp)
    highWord += 1; //Overflow

  rda1846SetFreq(highWord, lowWord);
}

