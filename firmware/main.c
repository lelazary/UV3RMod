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


unsigned char	selfBias;		//
unsigned char	i;

#define  	ADC_0		0x00           	// AN0	 
#define  	ADC_1		0x04           	// AN1	 
#define  	ADC_2		0x08          	// AN2	 
#define  	ADC_3		0x0C         	// AN3	  
#define  	ADC_4		0x10           	// AN4	 
#define  	ADC_5		0x14           	// AN5	 
#define  	ADC_BIAS	0x3C           	// AN15	 

unsigned char readADC(unsigned char ADC_CH)			// 8bit ADC read 
{
  unsigned char k;				//

  ADCRH  = 0x60;				// set 8bit ADC mode   
  ADCM   = ADC_CH + 0x82;			// conversion start		 			               	 
  for(k=0;k<0xFF;k++)			//
  {	if(ADSF) break;			//
  }						//
  return	ADCRL;			// return 8 bit data 
}							//


void getSelfBias(void)
{

  i	= readADC(ADC_BIAS);		// ADC_15 

  asm("	ldx	_i				;
      lda	#0CAh				; 3280 
      ldy	#0Ch				;
      div					;
      sta	_selfBias			;
      ");						//

}

static unsigned char encoderState = 0;

long freq = 0xFFFF;

int main()
{
  //Pin 31 is R10
  int i,j,k;

  initIOPorts();

  msDelay(100);
  getSelfBias();
  lcdInit(47); //Adjust this for LCD contrast


  unsigned char reg = 0;
  unsigned short b = '/';
  unsigned char pos = 0;

  R11 = 1; //Turn on back light
  lcdShowStr("HACKED",6);
  lcdShowStr(" UV3R ",0);
  msDelay(2000); //Show startup screen for 2 seconds
  short encoderPos = 0;

  //Initial RDA settings
  initRDA1846();

  lcdClear();

  while(1)
  {
    R11 = 1; //Turn on back light
    int k=0; 
    lcdShowStr("145525",6);

    //BUZR  = val++; 
    //Read from R23

    unsigned short val = SPITransfer(0x5F | 0x80, 0x0000);
    lcdShowNum(val & 0x03FF, 11, 16); //Show the results

    if (val > 0x0200) //We have a strong signal, send the DTMF tones
    {
      msDelay(2000);
      lcdShowStr("DTMF", 0);
      msDelay(2000);
      lcdShowStr("TX ON", 0);
      SPITransfer(0x1F, 0xC0);
      SPITransfer(0x63, 0x01F0 ); //00000001 00010001
      SPITransfer(0x30, 0x3046); //TX
      SPITransfer(0x35, 2855); 
      SPITransfer(0x36, 6049); 
      msDelay(1000);
      SPITransfer(0x35, 3154); 
      SPITransfer(0x36, 4952); 
      msDelay(1000);
      SPITransfer(0x35, 2855); 
      SPITransfer(0x36, 4952);
      msDelay(1000);
      SPITransfer(0x30, 0x302E); //RX
      SPITransfer(0x1F, 0x00);
      lcdShowStr("TX OFF", 0);
      for(k=0; k<10; k++)
        for(i=0; i<10000; i++)
          WDTR	= 0x9F;
      lcdClear();
    }
    else
    {
      lcdShowStr("WAIT", 0);
    }


    char encoderDir = getDialEncoder();
    if (encoderDir)
    {
      encoderPos += encoderDir;
      lcdClear();
      lcdShowNum(encoderPos, 5, 10);
      switch(encoderPos)
      {
        case 1:
          SPITransfer(0x30, 0x302E); //2E RX
          SPK_EN = 1;
          break;
        case 2:
          SPITransfer(0x30, 0x3046); //TX
          break;
        default:
          SPITransfer(0x30, 0x3006); //Stop eveything
          break;
      }

    }

    //unsigned char val = readADC(ADC_1); //Read the battery level
    WDTR	= 0x9F;
  }

}



