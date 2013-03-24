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
#include "uart.h"

unsigned char PAGE1 txBuffer[SERIAL_BUFFER_SIZE+5];
unsigned char PAGE1 txHead=0;
unsigned char PAGE1 txTail=0;

unsigned char PAGE1 rxBuffer[SERIAL_BUFFER_SIZE+5];
//unsigned char PAGE1 rxHead=0;
unsigned char PAGE1 rxTail=0;

unsigned int rxHead = 0;

void uartInit()
{
  //INIT UART
  ASIMR0  = 0xC2;     // 1100_0010b  no parity  org
  BRGCR0  = 0x3A;     // 0011_1010 BAUD_19200 @fx=8Mhz, BAUD_9600@4Mhz
  //BRGCR0  = 0x3A;     // 0011_1010 BAUD_19200 @fx=8Mhz, BAUD_9600@4Mhz
  //BRGCR0  = 0x08;     //BAUD_38400;   // ACK 
  //BRGCR0  = 0x4A;     //BAUD_9600 @fx=8Mhz,

}

unsigned char uartAvailable()
{
  return (SERIAL_BUFFER_SIZE + rxHead - rxTail) % SERIAL_BUFFER_SIZE;
}


short uartRead()
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (rxHead == rxTail)
  {
    return -1;
  } else {
    unsigned char c = rxBuffer[rxTail];
    rxTail = (rxTail+1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

unsigned char getChar()
{
  int i;
  for(i=0; i<1000 && !uartAvailable(); i++)
    WDTR	= 0x9F;

  if (uartAvailable())
    return uartRead();
  else
    return 0;
}

void uartIntHandler(void)
{

  if (ASISR0) //Check for errors
  {
    IFRX0 = 0;
    IFTX0 = 0;
    unsigned char t = RXBR; //need to read to flush out data per specs

    return;
  }


  if(IFRX0)
  {
    IFRX0 = 0;
    //LCD_BACKLIGHT = 1;
    if (rxHead+1 != rxTail) //We still have room in the buffer
    {
      rxBuffer[rxHead] = RXBR;
      rxHead = (rxHead + 1) % SERIAL_BUFFER_SIZE;
    }
  }

  if(IFTX0) 
  {
    IFTX0 = 0;
    txTail = (txTail + 1) % SERIAL_BUFFER_SIZE;
    if (txHead != txTail)
      TXSR = txBuffer[txTail];
  }
}

void uartSendMsg(char* str)
{
  //while(*str != 0)
  //  uartWrite(*str++);

  unsigned char i;
  
  //LCD_BACKLIGHT=1;
  //Wait for any previous tx to finish
  while(txHead != txTail)
    WDTR	= 0x9F;
 // LCD_BACKLIGHT=0;

  while(*str != 0)
  {
    txBuffer[txHead] = *str++;
    txHead = (txHead +1) % SERIAL_BUFFER_SIZE;
  }

  TXSR = txBuffer[txTail]; //Transmit the first char to trigger the int

  ////Wait for TX for finish
  //while(txHead != txTail)
  //  WDTR	= 0x9F;

}

void uartSendNum(unsigned short num, unsigned char base)
{
  char buf[20]; //sufficent for a short
  char* str = &buf[sizeof(buf)-1];

  *str = '\0';

  if (num == 0)
  {
    *--str = '0';
  } else {
    
    while(num > 0)
    {
      unsigned char digi = num % base;
      *--str = digi < 10 ? '0' + digi : 'A' + digi - 10;
      num /= base;
    }
  }
  uartSendMsg(str);
}


