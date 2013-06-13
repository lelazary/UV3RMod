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
#include "rda.h"
#include "radioModes.h"


unsigned char updateTime = 0;
unsigned char updateLoc = 0;

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
  switch(keys)
  {
    case VOL_KEY:
      updateLoc++;
      if (updateLoc > 6)
        updateLoc = 0;
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
        radioSettings.txTime = 0;
        LCD_BACKLIGHT = 0;
      }
      break;
  }

  switch(updateLoc)
  {
    case 0: resetEditLoc(); break;
    case 1: uiSetEditLoc(UI_TL, 0); break;
    case 2: uiSetEditLoc(UI_TL, 1); break;
    case 3: uiSetEditLoc(UI_TL, 2); break;
    case 4: uiSetEditLoc(UI_TR, 0); break;
    case 5: uiSetEditLoc(UI_TR, 1); break;
    case 6: uiSetEditLoc(UI_TR, 2); break;
  }

  if (encoderDir && (!radioSettings.transmitting)) //Dont change while transmitting
  {
    uiSetEditValue(encoderDir);
    updateRDA1846Freq(radioSettings.rxFreqM, radioSettings.rxFreqK);
  }

  initUI();
  if (radioSettings.transmitting)
  {
    uiAddBigNum(UI_TR, &radioSettings.txTime);
    radioSettings.txTime++;
    if (radioSettings.txTime > 10000)
    {
      radioSettings.transmitting = 0;
      rda1846RX(1);
      radioSettings.txTime = 0;
      LCD_BACKLIGHT = 0;
    }
  } else {
    uiAddSmallNum(UI_TL, &radioSettings.rxFreqM);
    uiAddSmallNum(UI_TR, &radioSettings.rxFreqK);
  }
  showUI();
    

}


