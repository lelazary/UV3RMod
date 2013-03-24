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
//Functions related to control the radio remotly
#include <MC81F8816/MC81F8816.h>
#include <hms800.h>
#include "lcd.h"
#include "time.h"
#include "uv3r.h"
#include "uart.h"
#include "ui.h"


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

void setReg(unsigned char addr, short data)
{
  uartSendMsg("Set2: ");
  uartSendNum(addr, 16);
  uartSendMsg(" to ");
  uartSendNum(data, 16);
  uartSendMsg("\r\n");
  
  //SPI(0x3C, 0x0958 ); //00000001 00010001
  //SPI(addr, data);
  SPI(addr, data ); 
}

void processSerialCommand()
{
  LCD_BACKLIGHT = 1;
  uartSendMsg("Check start\n");
  if (getChar() != 0xAA || getChar() != 0x55)
    return;
  uartSendMsg("Got start\n");
  unsigned char cmd = getChar();
  uartSendMsg("Got Command ");
  uartSendNum(cmd, 10);
  uartSendMsg("\n");

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
    case 'S':
      {
        unsigned char addr = getChar()&0xFF;

        short data = getChar()&0xFF;
        data <<= 8;
        data |= getChar()&0xFF;

        if(getChar() == '\r' && getChar() == '\n')
        {
          uartSendMsg("Set: ");
          uartSendNum(addr, 16);
          uartSendMsg(" to ");
          uartSendNum(data, 16);
          uartSendMsg("\r\n");

          setReg(addr, data); //, data);
        }
      }
      break;
  }
  LCD_BACKLIGHT = 0;

}

