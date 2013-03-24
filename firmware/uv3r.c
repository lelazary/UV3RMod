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

static unsigned char encoderState = 0;
unsigned char i;

// Read the dial encoder using gray code to avoid debouncing. 
//Insperations from
// http://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino
char getDialEncoder()
{
  //static char encStateTable[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static char encStateTable[] =   {0, 1,0,-1,-1,0,1, 0, 0,1,0,-1,-1,0,1,0};

  //Save the previous state by shifting it by 2 and inserting the
  //current pin A and B state
  //lcdShowNum(DIAL_B, 11, 16);
  //lcdShowNum(encoderState & 0x01, 5, 16);
  if (DIAL_B != (encoderState & 0x01)) //Check that DIAL_B has changed, Could be handle in INT
  {
    encoderState <<= 1; 
    if (DIAL_A) encoderState |= 0x01;
    encoderState <<= 1;
    if (DIAL_B) encoderState |= 0x01;
    //lcdShowStr("T", 6);
    //lcdShowNum( (encoderState & 0x0f), 5, 16);
    //msDelay(1000);
    //Look up in the table weather its a valid state and in which direction
    return encStateTable[ (encoderState & 0x0f) ];
  } else {
    //lcdShowStr("F", 6);
    return 0;
  }
}

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

void initIOPorts()
{
  //R00 Dial Data (Input + pullup)
  //R04 Key tone (output)
  //R05 Dial In  (Input + pullup)
  //R06 Power Key Input/RDA1846 sck output
  //R07 Keypad +? / CHG Det Input

  //                      Dial Data 
  R0IO = 0x50; //0101 0000
  R0PSR= 0x00; //0000 0000  //Disable Enable buzzer
  R0PU = 0x21; //0010 0001
  R0 = 0x00;


  //R10 Lap (Output)
  //R11 Backlight (Output)
  //R12 TXD (output)
  //R13 PPT/RXD (Input)
  //R14 GPIO6 (Input)
  //R15 RDA1846 SEN (Output)
  //R16 RDA1846 SDIO (INput/Output)
  //R17 Speker Enable (Output)

  R1IO=0xE7; 	// 1110 0111
  R1PSR = 0x00; //Normal pins

  //R20 Vox Det (Input analog)
  //R21 Bat Det (Input analog)
  //R22 RDA1846 GPIO0 (input)
  //R23 Key Pad (Input Analog)
  //R24 Pow SW (output)

  //Reg on, turn on the radio
  R2IO = 0x10; //0001 0000
  R2PU		= 0x00;			// off,  off,  off,  off,  off,  off		 				 
  R2OD		= 0x00;			// PP,   PP,   PP,   PP,   PP,   PP
  R2		  = 0x00;			//  0     0     0     0     0     0   		         
  // ADC   IO    IO    IO    IO    IO 

  RADIO_PW = 1; //Power on the radio
  SPK_EN = 0;  //Turn off the speaker
}


//Voltage for keys
//Idel 250-255
//vol 0
//menu 53
//u/v 108
//f/a 155
//l/r 207
//


unsigned char getKeys()
{

  static unsigned char integrator = 0;
  static unsigned char prevKeys = 0;
#define KEYS_ADC_OFFSET 10
  unsigned char keys = 0;

  unsigned char keysADC = readADC(ADC_3);
  if (keysADC > 255 - KEYS_ADC_OFFSET)
    keys |= 0; //no key presses
  else if (keysADC > 207 - KEYS_ADC_OFFSET)
    keys |= LR_KEY;  
  else if (keysADC > 155 - KEYS_ADC_OFFSET)
    keys |= FA_KEY;
  else if (keysADC > 100 - KEYS_ADC_OFFSET)
    keys |= UV_KEY;
  else if (keysADC > 50 - KEYS_ADC_OFFSET)
    keys |= MENU_KEY;
  else if (keysADC >= 0 - KEYS_ADC_OFFSET)
    keys |= VOL_KEY;

  if (!R13) //PPT button NC
    keys |= PTT_KEY;

#define INTMAX 50

  //Debounce using integrator
  if (prevKeys == keys)
  {
    if (integrator > 0)
      integrator--;
  }
  else if (integrator < INTMAX)
    integrator++;

  if (integrator == 0)
    return 0;
  else if (integrator >= INTMAX)
  {
    prevKeys = keys;
    integrator = INTMAX;
    return prevKeys;
  }

  return 0;

}
