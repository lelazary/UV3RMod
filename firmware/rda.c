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

void initRDA1846()
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
short getRSSI()
{
  short val = SPI(0x5F | 0x80, 0x0000) & 0x03FF;

  val >>= 3; //devide by 8
  val -= 135; //Per datasheet
  return val;
}

unsigned char getDTMF()
{
  return 0;
}

void txDTMF(unsigned char* values, unsigned int len, unsigned short delay)
{
  int i=0;
  //Set tx mode
  SPI(0x1F, 0xC0);
  SPI(0x63, 0x01F0 ); //00000001 00010001
  SPI(0x30, 0x3046); //TX
  
  for(i=0; i<len; i++)
  {
    //Get data from dtmf table
    SPI(0x35, 2855); 
    SPI(0x36, 6049); 
    msDelay(delay);
  }

  //Switch back to RX mode
  SPI(0x30, 0x302E); //RX
  SPI(0x1F, 0x00);
}

void setFreq(unsigned short freqU, unsigned short freqL)
{
    SPI(0x29, freqU);
    SPI(0x2A, freqL);
}

void tx()
{
  SPI(0x30, 0x3046); //TX
}

void rx(unsigned char useSq)
{
  if (useSq)
    SPI(0x30, 0x302E); //2E RX
  else
    SPI(0x30, 0x3026); //2E RX
}

void setPower(unsigned char power)
{

}
