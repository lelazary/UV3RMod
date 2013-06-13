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

char uiDisplay[13]; //Allow for null
char symbol[2];
char uiTopNum[2];

void* uiPtr[5];
char uiType[5] = {UI_NONE, UI_NONE, UI_NONE, UI_NONE, UI_NONE};
char uiEditLoc[5] = {-1, -1, -1, -1, -1};
char uiEditPos = -1;
unsigned char flashCnt = 0;
#define FLASH_INTERVAL 0x80


void showStr(unsigned char idx, void* ptr)
{
  char* str = (char*)ptr;
  unsigned char p = idx*3;
  while(*str != 0)
    uiDisplay[p++] = *str++;

  if (uiEditLoc[idx] != -1 && 
      ((flashCnt++)&FLASH_INTERVAL) )
  {
    uiDisplay[idx*3 + uiEditLoc[idx]] = '_';
  }
}

void showNum(unsigned char idx, void* ptr, unsigned char large)
{

  unsigned char base = 10;
  unsigned short num = *(unsigned short*)ptr;

  unsigned char uiIdx = idx*3;
  uiDisplay[uiIdx] =   ' ';
  uiDisplay[uiIdx+1] = ' ';
  uiDisplay[uiIdx+2] = ' ';
  if (large)
  {
    uiDisplay[uiIdx-1] =   ' ';
    uiDisplay[uiIdx-2] = ' ';
    uiDisplay[uiIdx-3] = ' ';
  }

  if (num == 0)
  {
    uiDisplay[uiIdx+2] = '0';
  } else {
    uiIdx += 2;
    while(num > 0)
    {
      unsigned char digi = num % base;
      digi = digi < 10 ? '0' + digi : 'A' + digi - 10;
      uiDisplay[uiIdx--] = digi;
      num /= base;
    }
  }

  if (uiEditLoc[idx] != -1 && 
      ((flashCnt++)&FLASH_INTERVAL) )
  {
    uiDisplay[idx*3 + (2-uiEditLoc[idx])] = '_';
  }
}

void showNumTop(unsigned char idx, void* ptr)
{

  unsigned short num = *(unsigned short*)ptr;

  uiTopNum[0] = ' ';
  uiTopNum[1] = ' ';

  unsigned char tens = num/10;
  unsigned char ones = num%10;

  uiTopNum[0] = '0' + tens;
  uiTopNum[1] = '0' + ones;

  if (uiEditLoc[idx] != -1 && 
      ((flashCnt++)&FLASH_INTERVAL) )
  {
    uiTopNum[1-uiEditLoc[idx]] = '_';
  }


}

void showUI()
{

  unsigned char i;
  for(i=0; i<5; i++)
  {
    switch(uiType[i])
    {
      case UI_NUM_SMALL:
        if (i==UI_TOP_NUM)
          showNumTop(i, uiPtr[i]);
        else
          showNum(i, uiPtr[i], 0);
        break;
      case UI_NUM_LARGE:
          showNum(i, uiPtr[i], 1);
        break;
      case UI_STR:
        showStr(i, uiPtr[i]);
        break;
    }
  }

  uiDisplay[12] = 0;
  lcdShowStr(uiDisplay, 0);
  //symbol[0],
  //symbol[1],
}

void initUI()
{
  unsigned char i=0;

  for(i=0; i<12; i++)
    uiDisplay[i] = '_';
  symbol[0] = ' ';
  symbol[1] = ' ';
  uiTopNum[0] = '_';
  uiTopNum[1] = '_';

}


void uiAddSmallNum(unsigned char pos, void* ptr)
{
  uiPtr[pos] = ptr;
  uiType[pos] = UI_NUM_SMALL;
}

void uiAddBigNum(unsigned char pos, void* ptr)
{
  uiPtr[pos] = ptr;
  uiType[pos] = UI_NUM_LARGE;
}

void uiAddStr(unsigned char pos, void* ptr)
{
  uiPtr[pos] = ptr;
  uiType[pos] = UI_STR;
}

void uiIncDecStr(unsigned char pos, char dir)
{
  if (uiType[pos] != UI_STR)
    return;

  char* str = (char*)uiPtr[pos];
  
  str[uiEditLoc[pos]] += dir;
  if (str[uiEditLoc[pos]] > 'z')
    str[uiEditLoc[pos]] = 0;
}

void uiIncDecNum(unsigned char pos, char dir)
{
  if (uiType[pos] != UI_NUM_SMALL)
    return;

  unsigned short* num = (unsigned short*)uiPtr[pos];

  switch(uiEditLoc[pos])
  {
    case 2:
      if (pos != UI_TOP_NUM)
      {
        if (dir > 0)
          *num += 100;
        else
          *num -= 100;
        break;
      }
    case 1:
      if (dir > 0)
        *num += 10;
      else
        *num -= 10;
      break;
    case 0:
      *num += dir;
      break;
  }

  if (pos == UI_TOP_NUM)
  {
    if (*num > 99) 
      *num = 99;

  } else {
    if (*num > 999) 
      *num = 999;
  }
}

void uiSetEditLoc(unsigned char pos, unsigned char loc)
{
  //TODO change to bitfield
  char i;
  for(i=0; i<5; i++)
    uiEditLoc[i] = -1;
  uiEditLoc[pos] = loc;
  uiEditPos = pos;
}

void resetEditLoc()
{
  char i;
  for(i=0; i<5; i++)
    uiEditLoc[i] = -1;
}

void uiSetEditValue(char dir)
{
  switch(uiType[uiEditPos])
  {
    case UI_NUM_SMALL:
    case UI_NUM_LARGE:
      uiIncDecNum(uiEditPos, dir);
      break;
  }


}


////////////////////////////////////////////////////////////////////////
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

