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
#include "uart.h"

#define TRUE 1
#define FALSE 0

unsigned char selfBias;
unsigned char	i;
unsigned short timer = 0;
unsigned char updateTime = 0;

CODE struct RadioSettings
{
  short rxFreqM; //The receive mega portion of the freq (BCD)
  short rxFreqK; //The receive kilo portion of the freq (BCD)
  short txFreqM; //The transmit mega portion of the freq (BCD)
  short txFreqK; //The transmit kilo portion of the freq (BCD)
  char offset;
  short rssi; //
  short vssi; //
  short dtmf; //
  short flags; //
  unsigned short ctcss; //
  //unsigned char contrast;
  //unsigned char power;
  //unsigned char volume;
  //unsigned char ctcss;
  //unsigned char sqOpen;
  //unsigned char sqClose;
  unsigned char txDTMF[6]; //8 different dtmf transmissions
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
  
  radioSettings.rssi = 0;
  radioSettings.vssi = 0;
  radioSettings.dtmf = 0;
  radioSettings.flags = 0;

  radioSettings.ctcss = 0;


  radioSettings.txDTMF[0] = 0x01;
  radioSettings.txDTMF[1] = 0x02;
  radioSettings.txDTMF[2] = 0x03;
  radioSettings.txDTMF[3] = 0x10; //Blank
  radioSettings.txDTMF[4] = 0x10; //Blank
  radioSettings.txDTMF[5] = 0x10; //Blank

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
  //lcdShowNum(radioSettings.ctcss, 5, 10);
  if (timer > 0)
  {
    lcdShowNum(timer,5, 10);
  }
  else
    lcdShowStr("1273PL",0);
  lcdSetSymbol('.', 0); //symbols need to be last
}

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

enum DISPLAY_MODE {
  FREQ_DISPLAY,
  STATUS_DISPLAY,
  DTMF_DISPLAY,
  MAX_DISPLAY_MODE}; //Last one used to know the size of the enum

void updateDisplay(unsigned char mode)
{

  if (!(updateTime++%100)) //Update the display every 100 loops so it will not fliker
  {
    switch(mode)
    {
      case FREQ_DISPLAY:
        showFreqDisplayMode(radioSettings.transmitting);
        break;
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

unsigned char changeMode = 0;
unsigned char displayMode = FREQ_DISPLAY;

unsigned char getChar()
{
  while(!uartAvailable())
    WDTR	= 0x9F;
  return uartRead();
}

void getFreqFromSerial(unsigned short* freqM, unsigned short* freqK)
{

  *freqM  = getChar()&0xFF;
  *freqM  <<= 8; 
  *freqM  |= getChar()&0xFF;

  *freqK  = getChar()&0xFF;
  *freqK  <<= 8; 
  *freqK  |= getChar()&0xFF;
  
  if(getChar() == '\r' && getChar() == '\n')
  {
    uartSendMsg("Set freqM: ");
    uartSendNum(*freqM, 10);
    uartSendMsg("\r\n");

    uartSendMsg("Set freqM: ");
    uartSendNum(*freqK, 10);
    uartSendMsg("\r\n");
  }

  return;


}

void processSerialCommand()
{
  LCD_BACKLIGHT = 1;
  unsigned char cmd = uartRead();
  switch(cmd)
  {
    case 'F':
      getFreqFromSerial(&radioSettings.rxFreqM, &radioSettings.rxFreqK);
      updateRDA1846Freq(radioSettings.rxFreqM, radioSettings.rxFreqK);
      break;
    case 'T':
      if(getChar() == '\r' && getChar() == '\n')
        rda1846TX();
      break;
    case 'R':
      if(getChar() == '\r' && getChar() == '\n')
        rda1846RX(1);
      break;
    case 'D':
      if(getChar() == '\r' && getChar() == '\n')
        rda1846TXDTMF(radioSettings.txDTMF, 6, 1000);
      break;
  }
  LCD_BACKLIGHT = 0;

}



int main()
{
  //Pin 31 is R10
  initIOPorts();

  IENH  = 0x0C;     //  x, INT0(6), INT1(5), INT2(4),RX(3),TX(2),x,x  // TX/RX enable 
  IENM    = 0x80;     // T0E(7),T1E(6),T2E(5),T3E(4), -, -, -, ADCE(0) 
  IENL    = 0x10;     // SPIE(7),BITE(6),WDTE(5),WTE(4),INT3(3),I2CE(2),x,x               
  asm(" 
      clrg          ;
      EI          ; Enable global interrupt 
      nop         ; 
      ");


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

  int num = 0;
  rda1846RX(1);

  uartInit();
  
  unsigned char val=0;


  while(1)
  {
    WDTR	= 0x9F;
    int k=0; 

    unsigned char avl  = uartAvailable();
    radioSettings.ctcss=avl; //TODO temp
    if (avl > 0)
      processSerialCommand();

    if (radioSettings.transmitting)
    {
      timer++;
      if (timer > 10000)
      {
        radioSettings.transmitting = 0;
        rda1846RX(1);
        timer = 0;
        LCD_BACKLIGHT = 0;
      }
    }
      

    unsigned char keys = getKeys();
    if (keys)
    {
      switch(keys)
      {
        case VOL_KEY:
          changeMode++;
          if (changeMode > 6)
            changeMode = 0;
          if (displayMode == FREQ_DISPLAY)
            lcdSetFlashPos(changeMode+6);
          else if (displayMode == DTMF_DISPLAY)
            lcdSetFlashPos(changeMode);

          break;
        case MENU_KEY:
          displayMode++;
          if (displayMode >= MAX_DISPLAY_MODE)
            displayMode = 0;
          break;
        case PTT_KEY:
          if (displayMode == FREQ_DISPLAY)
          {
            radioSettings.transmitting = !radioSettings.transmitting;
            if (radioSettings.transmitting)
            {
              LCD_BACKLIGHT = 1;
              rda1846TX();
            }
            else
            {
              rda1846RX(1);
              timer = 0;
              LCD_BACKLIGHT = 0;
            }
            break;
          } 
          else if (displayMode == DTMF_DISPLAY)
          {
            rda1846TXDTMF(radioSettings.txDTMF, 6, 1000);
          }

      }
    } else {
      //Update status
      short rssi, vssi, dtmf, flags;
      rda1846GetStatus(
          &radioSettings.rssi,
          &radioSettings.vssi);
          //&dtmf,
          //&flags);
      //radioSettings.transmitting = 0;
    }

    
    char encoderDir = getDialEncoder();
    if (encoderDir)
    {
      if (!radioSettings.transmitting) //Dont change while transmitting
      {

        if (displayMode == FREQ_DISPLAY)
        {
          if (changeMode > 5)
            radioSettings.offset += encoderDir;
          if (changeMode > 2)
            updateNum(&radioSettings.rxFreqK, changeMode-3, encoderDir);
          else
            updateNum(&radioSettings.rxFreqM, changeMode, encoderDir);

          updateRDA1846Freq(radioSettings.rxFreqM, radioSettings.rxFreqK);
        }
        else if (displayMode == DTMF_DISPLAY)
        {
          if (changeMode > 5)
            changeMode = 5;
          radioSettings.txDTMF[changeMode] += encoderDir;
          if (radioSettings.txDTMF[changeMode] > 0x10)
            radioSettings.txDTMF[changeMode] = 0x10;
        }
      }
    }
    

    radioSettings.txFreqM = radioSettings.rxFreqM + radioSettings.offset;
    radioSettings.txFreqK = radioSettings.rxFreqK; 

    updateDisplay(displayMode);

    //unsigned char val = readADC(ADC_1); //Read the battery level
  }

  return 0;
}



