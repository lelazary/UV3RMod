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
#include "util.h"
unsigned char flashPos = 0xFF;
unsigned char flashTime = 0;

CODE unsigned char numFont[][2] = 
{
  0xDB, 0x7E,
  0x0A, 0x60,
  0xBD, 0x5A,
  0xAF, 0x78,
  0x6E, 0x64,
  0xE7, 0x3C,
  0xF7, 0x3E, //77 36 0111 0111  0011 0110
  0x8A, 0x68,
  0xFF, 0x7E,
  0xEE, 0x6C,
};

CODE unsigned char alphaNumFont[][4] = 
{
0x00, 0x00, 0x00, 0x00,   //  Lower: Blank Upper: Blank
0x00, 0x48, 0x22, 0x00,   //  Lower: /  0000 1000 0010 0000 Upper: /  0000 0100 0010 0000
0x5B, 0x48, 0xBA, 0x5A,   //  Lower: 0  1011 1000 1010 1010 Upper: 0  0101 0100 1011 0101
0x5A, 0x00, 0x00, 0x00,   //  Lower: 1  1010 0000 0000 0000 Upper: 1  0101 0000 0000 0000
0x6D, 0x00, 0x98, 0x36,   //  Lower: 2  1101 0000 1000 0110 Upper: 2  0110 0000 1001 0011
0x7F, 0x00, 0x98, 0x00,   //  Lower: 3  1111 0000 1000 0000 Upper: 3  0111 0000 1001 0000
0x7E, 0x00, 0x00, 0x6C,   //  Lower: 4  1110 0000 0000 1100 Upper: 4  0111 0000 0000 0110
0x01, 0x12, 0x98, 0x6C,   //  Lower: 5  0001 0010 1000 1100 Upper: 5  0000 0001 1001 0110
0x37, 0x00, 0x98, 0x7E,   //  Lower: 6  0111 0000 1000 1110 Upper: 6  0011 0000 1001 0111
0x5A, 0x00, 0x88, 0x00,   //  Lower: 7  1010 0000 1000 0000 Upper: 7  0101 0000 1000 0000
0x7F, 0x00, 0x98, 0x7E,   //  Lower: 8  1111 0000 1000 1110 Upper: 8  0111 0000 1001 0111
0x7F, 0x00, 0x98, 0x6C,   //  Lower: 9  1111 0000 1000 1100 Upper: 9  0111 0000 1001 0110
0x00, 0x24, 0x00, 0x00,   //  Lower: :  0000 0100 0000 0000 Upper: :  0000 0010 0000 0000
0x00, 0x24, 0x22, 0x00,   //  Lower: ;  0000 0100 0010 0000 Upper: ;  0000 0010 0010 0000
0x00, 0x5A, 0x00, 0x00,   //  Lower: <  0000 1010 0000 0000 Upper: <  0000 0101 0000 0000
0x25, 0x00, 0x10, 0x24,   //  Lower: =  0101 0000 0000 0100 Upper: =  0010 0000 0001 0010
0x00, 0x00, 0x66, 0x00,   //  Lower: >  0000 0000 0110 0000 Upper: >  0000 0000 0110 0000
0x6C, 0x00, 0x88, 0x36,   //  Lower: ?  1100 0000 1000 0110 Upper: ?  0110 0000 1000 0011
0x6D, 0x48, 0x98, 0x5A,   //  Lower: @  1101 1000 1000 1010 Upper: @  0110 0100 1001 0101
                              
0x7E, 0x00, 0x88, 0x7E,   //  Lower: A  1110 0000 1000 1110 Upper: A  0111 0000 1000 0111
0x7F, 0x24, 0x98, 0x00,   //  Lower: B  1111 0100 1000 0000 Upper: B  0111 0010 1001 0000
0x01, 0x00, 0x98, 0x5A,   //  Lower: C  0001 0000 1000 1010 Upper: C  0000 0000 1001 0101
0x5B, 0x24, 0x98, 0x00,   //  Lower: D  1011 0100 1000 0000 Upper: D  0101 0010 1001 0000
0x25, 0x00, 0x98, 0x7E,   //  Lower: E  0101 0000 1000 1110 Upper: E  0010 0000 1001 0111
0x00, 0x00, 0x88, 0x7E,   //  Lower: F  0000 0000 1000 1110 Upper: F  0000 0000 1000 0111
0x37, 0x00, 0x98, 0x5A,   //  Lower: G  0111 0000 1000 1010 Upper: G  0011 0000 1001 0101
0x7E, 0x00, 0x00, 0x7E,   //  Lower: H  1110 0000 0000 1110 Upper: H  0111 0000 0000 0111
0x01, 0x24, 0x98, 0x00,   //  Lower: I  0001 0100 1000 0000 Upper: I  0000 0010 1001 0000
0x5B, 0x00, 0x10, 0x12,   //  Lower: J  1011 0000 0000 0010 Upper: J  0101 0000 0001 0001
0x00, 0x5A, 0x00, 0x7E,   //  Lower: K  0000 1010 0000 1110 Upper: K  0000 0101 0000 0111
0x01, 0x00, 0x10, 0x5A,   //  Lower: L  0001 0000 0000 1010 Upper: L  0000 0000 0001 0101
0x5A, 0x48, 0x44, 0x5A,   //  Lower: M  1010 1000 0100 1010 Upper: M  0101 0100 0100 0101
0x5A, 0x12, 0x44, 0x5A,   //  Lower: N  1010 0010 0100 1010 Upper: N  0101 0001 0100 0101
0x5B, 0x00, 0x98, 0x5A,   //  Lower: O  1011 0000 1000 1010 Upper: O  0101 0000 1001 0101
0x6C, 0x00, 0x88, 0x7E,   //  Lower: P  1100 0000 1000 1110 Upper: P  0110 0000 1000 0111
0x5B, 0x12, 0x98, 0x5A,   //  Lower: Q  1011 0010 1000 1010 Upper: Q  0101 0001 1001 0101
0x6C, 0x12, 0x88, 0x7E,   //  Lower: R  1100 0010 1000 1110 Upper: R  0110 0001 1000 0111
0x37, 0x00, 0x98, 0x6C,   //  Lower: S  0111 0000 1000 1100 Upper: S  0011 0000 1001 0110
0x00, 0x24, 0x88, 0x00,   //  Lower: T  0000 0100 1000 0000 Upper: T  0000 0010 1000 0000
0x5B, 0x00, 0x10, 0x5A,   //  Lower: U  1011 0000 0000 1010 Upper: U  0101 0000 0001 0101
0x00, 0x48, 0x22, 0x5A,   //  Lower: V  0000 1000 0010 1010 Upper: V  0000 0100 0010 0101
0x5A, 0x12, 0x22, 0x5A,   //  Lower: W  1010 0010 0010 1010 Upper: W  0101 0001 0010 0101
0x00, 0x5A, 0x66, 0x00,   //  Lower: X  0000 1010 0110 0000 Upper: X  0000 0101 0110 0000
0x00, 0x48, 0x66, 0x00,   //  Lower: Y  0000 1000 0110 0000 Upper: Y  0000 0100 0110 0000
0x01, 0x48, 0xBA, 0x00,   //  Lower: Z  0001 1000 1010 0000 Upper: Z  0000 0100 1011 0000
0x01, 0x48, 0xBA, 0x00,   //  Lower: [  0001 0000 1000 0000 Upper: [  0000 0000 1011 0000 //TODO
0x01, 0x48, 0xBA, 0x00,   //  Lower: \  0001 0000 1000 0000 Upper: \  0000 0000 1011 0000 //TODO
0x01, 0x48, 0xBA, 0x00,   //  Lower: ]  0001 0000 1000 0000 Upper: ]  0000 0000 1011 0000 //TODO
0x01, 0x48, 0xBA, 0x00,   //  Lower: ^  0001 0000 1000 0000 Upper: ^  0000 0000 1011 0000 //TODO
0x01, 0x00, 0x10, 0x00,   //  Lower: _  0001 0000 0000 0000 Upper: _  0000 0000 0001 0000
};

void lcdSetSymbol(unsigned char sym, unsigned char lower)
{
  if (sym == '.')
  {
    SEG14 |= 0x81;
  }


}

void lcdSetLevel(unsigned char level)
{
   
}

void lcdSetFrac(unsigned char frac)
{

}

void lcdSmallNumber(unsigned char num)
{
  
  unsigned char tens = num/10;
  unsigned char ones = num%10;

  SEG0 = (numFont[tens][0]&0x0F) | numFont[ones][0]&0xF0;
  SEG1 = (numFont[tens][1]&0x0F) | numFont[ones][1]&0xF0;

}


void lcdAlphaNum(unsigned char pos, unsigned char c)
{
	unsigned char *ptr = LCD_ADDR + 2 + ((pos%6)*4);  //The first 2 seg are for the small number


  if (flashPos == pos &&
      ((flashTime++)&0x04)) //turn on and off evey 4 cycles
    c= '_';

  if (c == ' ')
    c = 0;
  else
    c = c - '/' + 1; //We start with a slash, and the first one is space


	if (pos > 5) //upper segment
    {
     *(ptr+0) = *(ptr+0)&0x0F | alphaNumFont[c][3]&0xF0;
     *(ptr+1) = *(ptr+1)&0x0F | alphaNumFont[c][2]&0xF0;
     *(ptr+2) = *(ptr+2)&0x0F | alphaNumFont[c][1]&0xF0;
     *(ptr+3) = *(ptr+3)&0x0F | alphaNumFont[c][0]&0xF0;
    } else { //lower segment
     *(ptr+0) = *(ptr+0)&0xF0 | alphaNumFont[c][3]&0x0F;
     *(ptr+1) = *(ptr+1)&0xF0 | alphaNumFont[c][2]&0x0F;
     *(ptr+2) = *(ptr+2)&0xF0 | alphaNumFont[c][1]&0x0F;
     *(ptr+3) = *(ptr+3)&0xF0 | alphaNumFont[c][0]&0x0F;
	}
    
 }


void lcdInit(unsigned char rSelf_Volt)
{


	R5IO		= 0xFF;			//							 
	R5		    = 0x00;			//   							 
	R5PSR		= 0x00;			// R5 LCD Segment out     

	R6IO		= 0xFF;			//							 
	R6		    = 0x00;			//   							 
	R6PSR		= 0x00;			// R6 LCD Segment out     

	R7IO		= 0xFF;			//							 
	R7		    = 0x00;			//   							 
	R7PSR		= 0x00;			// R7 LCD Segment out     

	WTMR		= 0x99;			// WDT load enable 0001 1001 	//Needed for LCD clock
   

	LCR	= 0x68	;			// 4COM internal bias LCD   1110 1100

	lcdClear();

	//LBCR  = 0x9C;			// 1001_1100b (Vdd)

//	LBCR  = 0xFA;			// 1111_1010b (Vdd)
	if(rSelf_Volt>47)			// 
		LBCR	= 0x89;		// contrast Vdd/2 + Vdd/30  					x000_1xxx
	else 
	{	if(rSelf_Volt>42)
			LBCR	= 0xB1;	// contrast  Vdd/2 + Vdd/20  				x011_0xxx
		else 
		{	if(rSelf_Volt>36)
				LBCR	= 0xD9;	// contrast control Vdd/2 + Vdd/10 		x101_1xxx
			else
			{	if(rSelf_Volt>30)
					LBCR	= 0xF1;	// contrast control Vdd/2 + Vdd/4  	x111_0xxx
				else 	LBCR	= 0xF9;		// contrast Vdd 			x111_1xxx 
			}
		}
	} 
		
}

void lcdClear()
{
 	unsigned char *ptr = LCD_ADDR;  //The first 2 seg are for the small number
	unsigned char i=0; 

	//Clear the LCD
	for(i=0; i<40; i++)
	  *(ptr+i) = 0;

}

unsigned char lcdShowNum(unsigned short num, unsigned char pos, unsigned char base)
{
  if (num == 0)
  {
    lcdAlphaNum(pos, '0');
    return pos;
  }
  while(num > 0)
  {
    unsigned char digi = num % base;
    digi = digi < 10 ? '0' + digi : 'A' + digi - 10;
    lcdAlphaNum(pos--, digi);
    num /= base;
  }
  return pos;
}

void lcdShowFixNum(unsigned short num, unsigned char top)
{
  unsigned short holeNum = num >> 8;
  lcdShowNum(holeNum, 2, 10);
  lcdSetSymbol('.', 0);
  unsigned char i=0;
  for(i=0; i<3; i++)
  {
    num &= 0x00FF; //Get the float part
    num *= 10; //Scale by 10 
    unsigned char val = num >> 8; //Get the integer part
    lcdAlphaNum(3+i, '0' + val );
  }
}

void lcdShowFrac(unsigned short num, unsigned char numDigits, unsigned char round)
{

  unsigned char i=0;
  char str[7]; 
  for(i=0; i<numDigits; i++) //Only 3 decimal digits
  {
    struct Fix32Num r = fixMultInt(num, 10);
    str[i] = '0' + (r.high&0x000F);
    num = r.low;
  }
  str[i] = 0;

  if (round)
  {
    struct Fix32Num r = fixMultInt(num, 10);
    i--;
    if ((r.high&0x000F) > 5) //Round the lsd
      str[i]++;
    for(;i>0; i--) //Go back and inc any digits > 10
    {
      if (str[i] > '9')
      {
        str[i] = '0';
        str[i-1]++;
      }
    }
  }

  lcdShowStr(str, 0);
}


void lcdShowStr(char* str, unsigned char pos)
{
  while(*str != 0)
     lcdAlphaNum(pos++, *str++);
}

void lcdSetFlashPos(unsigned char pos)
{
  flashPos = pos;
}
