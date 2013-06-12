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
#include "util.h"

unsigned short wDly_count;

void delay(unsigned short value); //delay in usec
void msDelay(unsigned short value);

//---------------------------------------------------------------
//	N ms delay 	by 4MHz crystal 	
//
//	(caution!) its only aprox because the loop is not accounted for
void msDelay(unsigned short value)
{
   unsigned short i;
   for(i=0; i<value; i++) 
   {  
      delay(1000);
      WDTR	= 0x9F; //reset the watch dog timer
   }

}

//---------------------------------------------------------------
//	N usec delay 	by 4MHz crystal 	
//
//	(caution!) It is available over 48us delay 
void delay(unsigned short value)		 
{
	wDly_count = value-30;		// 30 us 

#ifndef SIM
 asm("
 	lsr	_wDly_count+1		; 4	1/8 
	ror	_wDly_count			; 4
 	lsr	_wDly_count+1		; 4
	ror	_wDly_count			; 4
 	lsr	_wDly_count+1		; 4
	ror	_wDly_count			; 4

 Rpt_dly:
	decw	_wDly_count			; 6
	nop					; 2
	nop					; 2
	nop					; 2
	bne	Rpt_dly			; 4	500ns x 16 = 8us  
    ");
#endif
}

struct Fix32Num fixMultInt(unsigned short fixNum1, unsigned short fixNum2)
{
  //From http://www.cs.uaf.edu/~cs301/notes/Chapter5/node5.html
  //Multiply two fix point short numbers and place in high/low register
  
  //Break down if fixNum1 contain a.b where a is the int and b is the fraction then
  //a.b * c.d = a*c + a*d + b*c + b*d
  //If we just want to the top 8 bits then
  //a*c + (a*d >> 8) + (b*c >> 8) 
  //TODO: could check the overflow bit, but too lazy

  unsigned char a = (fixNum1 >> 8)&0xff;
  unsigned char b = fixNum1&0xff;

  unsigned char c = (fixNum2 >> 8)&0xff;
  unsigned char d = fixNum2&0xff;

  struct Fix32Num result;
  result.low = b*d;

  unsigned short x1 = a*d;
  unsigned short x2 = c*b;

  unsigned short x = x1 + x2;
  unsigned char overflow = 0;
  if (x < x1)
    overflow = 1;

  unsigned short y = ((result.low >> 8) & 0xff) + x;
  if (y < x) //Check for overflow
    overflow = 1;

  result.low =  ((y & 0xff) << 8); 
  result.low |= (result.low & 0xff);

  result.high = (y >> 8) & 0xff; 
  if (overflow)
    result.high |= 0x0100;

  unsigned short tmp = a*c;  
  result.high += tmp;

  return result;
}


//void display(unsigned short num)
//{
//  #define numDigit 3
//  printf("%i", num >> 8);
//  printf(".");
//  for(unsigned char i=0; i<numDigit; i++)
//  {
//    num &= 0x00FF; //Get the float part
//    num *= 10; //Scale by 10
//    unsigned char val = num >> 8; //Get the integer part
//    printf("%c", val + '0');
//  }
//
//  printf("\n");
//}

