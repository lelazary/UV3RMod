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
#include "rda.h"
#include "spi.h"
#include "uv3r.h"

void rda1846Init()
{
  int i;
  RDA1846_SEN = 1;
  RDA1846_SCK = 1;
  for(i=0; i<30; i++)
  {
    SPI(RDAinit[i].address, RDAinit[i].data);
  }
}

//Get the signal stength indications
void rda1846GetStatus(short* rssi, short* vssi) //, short* dtmf, short* flags)
{
  *rssi = SPI(0x5F | 0x80, 0x0000) & 0x03FF;
  *rssi >>= 3; //devide by 8
  *rssi -= 135; //Per datasheet
  *rssi *= -1;

  *vssi = SPI(0x60 | 0x80, 0x0000) & 0x7FFF; 

  //*dtmf = SPI(0x6C | 0x80, 0x0000);

  //*flags = SPI(0x5C | 0x80, 0x0000);

}

unsigned char getDTMF()
{
  return 0;
}

//GPIO0 css_out
//GPIO1 off VFM
//GPIO2 on  VHF
//GPIO3 off UHF
//GPIO4 on  VRX
//GPIO5 off TX-VCC
//GPIO6 SQ
//GPIO7 TX LED (red)

//1101 1010 1010 1001

void rda1846SetGPIO(unsigned char gpio)
{
  rda1846GPIO = 0; //Should we reset each time

  if (gpio & TX_LED)
    rda1846GPIO = 0xC0;
 // if (gpio & TX_

  //{ 0x1F, 0x1EB9 },        //GPIO selection 0001 1110 1011 1001
      
  SPI(0x1F, rda1846GPIO);
}

void rda1846TXDTMF(unsigned char* values, unsigned int len, unsigned short delay)
{
  int i=0;
  //Set tx mode
  SPI(0x1F, 0xC000);
  SPI(0x63, 0x01F0 ); //00000001 00010001
  SPI(0x30, 0x3046); //TX
  
  for(i=0; i<len; i++)
  {
    if (values[i] < 0x10)
    {
      //Get data from dtmf table
      SPI(0x35, dtmfTone[values[i]][0]); 
      SPI(0x36, dtmfTone[values[i]][1]); 
      msDelay(delay);
    }
  }

  //Switch back to RX mode
  SPI(0x30, 0x302E); //RX
  SPI(0x1F, 0x0000);
}

void rda1846SetFreq(unsigned short freqU, unsigned short freqL)
{
    SPI(0x29, freqU);
    SPI(0x2A, freqL);
}

void rda1846TX()
{
  SPI(0x3C, 0x0958 ); //00000001 00010001
  SPI(0x1F, 0xC000);
  SPI(0x30, 0x3046); //TX
}

void rda1846RX(unsigned char useSq)
{
  SPI(0x1F, 0);
  if (useSq)
    SPI(0x30, 0x302E); //2E RX
  else
    SPI(0x30, 0x3026); //2E RX
}

void rda1946SetPower(unsigned char power)
{

}
