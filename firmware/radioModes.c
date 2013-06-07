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
#include "radioModes.h"

void processRadioMode(unsigned char mode, unsigned char keys, char encoderDir)
{
  switch (mode)
  {
    case SIMPLE:
      simpleRadioMode(keys, encoderDir);
      break;
  }
}

void simpleRadioMode(unsigned char keys, char encoderDir)
{
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


  if (keys)
  {
    switch(keys)
    {
      case VOL_KEY:
        changeMode++;
        if (changeMode > 6)
          changeMode = 0;
        lcdSetFlashPos(changeMode+6);
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
  }

  if (encoderDir && (!radioSettings.transmitting)) //Dont change while transmitting
  {
    if (changeMode > 5)
      radioSettings.offset += encoderDir;
    if (changeMode > 2)
      updateNum(&radioSettings.rxFreqK, changeMode-3, encoderDir);
    else
      updateNum(&radioSettings.rxFreqM, changeMode, encoderDir);

    updateRDA1846Freq(radioSettings.rxFreqM, radioSettings.rxFreqK);
  }


  radioSettings.txFreqM = radioSettings.rxFreqM + radioSettings.offset;
  radioSettings.txFreqK = radioSettings.rxFreqK; 

  updateDisplay(displayMode);
}


