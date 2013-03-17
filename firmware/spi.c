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
#include "uv3r.h"

unsigned short SPI(unsigned char address, short data)
{
  unsigned char counter;
  int i;

  R1IO=0xE7; 			//1110 0011//Speaker enable Pin 31 output R10
  //Send R/W

  unsigned char read = address & 0x80;

  RDA1846_SEN = 0; //Enable the serial comm

  for(counter = 8; counter; counter--)
  {
    if (address & 0x80)
      RDA1846_SDIO = 1;
    else
      RDA1846_SDIO = 0; 

    RDA1846_SCK = 0;
    for(i=0; i<100; i++)WDTR	= 0x9F;;
    RDA1846_SCK = 1;
    for(i=0; i<100; i++)WDTR	= 0x9F;;

    address <<= 1;
  }


  if (read)
  {
    R1IO=0xA7; 			//1010 0011//Speaker enable Pin 31 output R10 	
    data = 0;
    for(i=0; i<100; i++)  //Delay
      WDTR	= 0x9F;;

    for(counter = 16; counter; counter--)
    {
      data <<= 1;
      RDA1846_SCK = 0;
      for(i=0; i<100; i++)WDTR	= 0x9F;;
      RDA1846_SCK = 1;
      if (RDA1846_SDIO)
        data |= 0x01;
      for(i=0; i<100; i++)WDTR	= 0x9F;;
    }

  } else {

    for(counter = 16; counter; counter--)
    {
      RDA1846_SCK = 0;

      if (data & 0x8000)
        RDA1846_SDIO = 1;
      else
        RDA1846_SDIO = 0; 

      RDA1846_SCK = 0;
      for(i=0; i<100; i++)WDTR	= 0x9F;;
      RDA1846_SCK = 1;
      for(i=0; i<100; i++)WDTR	= 0x9F;;

      data <<= 1;
    }
  }
  R1IO=0xE7; 			//1110 0011//Speaker enable Pin 31 output R10
  RDA1846_SEN = 1;

  return data;
}

