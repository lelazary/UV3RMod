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
#include "ui.h"

unsigned char selfBias;
unsigned char	i;

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
  lcdShowStr("002",3);
  msDelay(1000); //Show startup screen for 1 second

  //Initial RDA settings
  rda1846Init();
  rda1846RX(1); //Go into rx mode

  lcdClear();

  uartInit();

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
      radioSettings.txTime++;
      if (radioSettings.txTime > 10000)
      {
        radioSettings.transmitting = 0;
        rda1846RX(1);
        radioSettings.txTime = 0;
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
              radioSettings.txTime = 0;
              LCD_BACKLIGHT = 0;
            }
            break;
          } 
          else if (displayMode == DTMF_DISPLAY)
          {
            rda1846TXDTMF(radioSettings.txDTMF, 6, 1000);
          }
          break;

      }
    } else {
      //Update status
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



