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



#ifndef UV3R_H
#define UV3R_H



/*************************************
  Pin Function
  54/COM0  LCD C4
  55/COM1  LCD C3 
  56/COM2  LCD C2
  57/COM3           LCD C1
  58/COM4/SEG39     LCD S31
  59/COM5/SEG38  LCD S30
  60/COM6/SEG37  LCD S29
  61/COM7/SEG36  LCD S28
  62/SEG27  LCD S27
  63/SEG26  LCD S26
  64/SEG25  LCD S25
  1/SEG24   LCD S24
  2/SEG23   LCD S23
  3/SEG22   LCD S22
  4/SEG21   LCD S21
  5/SEG20   LCD S20
  6/SEG19   LCD S19
  7/SEG18   LCD S18
  8/SEG17   LCD S17
  9/SEG16   LCD S16
  10/SEG15  LCD S15
  11/SEG14  LCD S14
  12/SEG13  LCD S13
  13/SEG12  LCD S12
  14/SEG11  LCD S11
  15/SEG10  LCD S10
  16/SEG9  LCD S9
  17/SEG8  LCD S8
  18/SEG7  LCD S7
  19/SEG6  LCD S6
  20/SEG5  LCD S5 
  21/SEG4  LCD S4
  22/SEG3  LCD S3
  23/SEG2  LCD S2
  24/SEG1  LCD S1
  25/SEG0  LCD S0

  26/R00/PWM0/T0O  Dial Data
  27/R04/BUZO  Key Tone
  28/R05/INT0/EC1  Dial Int
  29/R06/INT1  Power Key/RDA1846 SCLK
  30/R07/INT2  Key Pad/CHG Det  (Input Read whether the battery is beeing charged) (Should be up for reading keypad?)
  31/R10/PWM1/T20  LAMP (Output)
  32/R11/ACK/SCK  Backlight (Output)
  33/R12/TX/SOUT  TXD
  34/R13/RX/SI  PPT/RXD
  35/R14  RDA1846 GPIO6 (SQ)/Green LED  (Input) sq (Green LED should turn on by itself)
  36/R15  RDA1846 SEN
  37/R16/SDA  RDA1846 SDIO
  38/R17/SCL  Speker Enable
  39/R20/AN0  VOX Det
  40/R21/AN1  BAt Det (Battery voltage)
  41/R22/AN2  RDA1846 GPIO0
  42/R23/AN3  Key PAD
  43/R24/AN4  RDA1846 PDN/REG_SW (Turn on the radio)
  44/AVref  VCC
  45/R42  FM_SDA/EEprom SCL
  46/PLLC  GND
  47/VDD  VCC
  48/R43/SXIN  EEprom SDA
  49/R44/SXOUT  FM_SCK
  50/VSS  GND
  51  XTL
  52  XTL
  53/R47  RESET

*/

//UV3R defs
char getDialEncoder(); 
void initIOPorts();
unsigned char getKeys(); 
char getBatteryLevel();
unsigned char readADC(unsigned char ADC_CH);			// 8bit ADC read 
void getSelfBias(void);

#define SPK_EN R17

#define RDA1846_SEN R15
#define RDA1846_SCK R06
#define RDA1846_SDIO R16
#define RDA1846_SQ R14

#define RADIO_PW R24

#define DIAL_A R00
#define DIAL_B R05

#define LCD_BACKLIGHT R11

#define  	ADC_0		0x00           	// AN0	 
#define  	ADC_1		0x04           	// AN1	 
#define  	ADC_2		0x08          	// AN2	 
#define  	ADC_3		0x0C         	// AN3	  
#define  	ADC_4		0x10           	// AN4	 
#define  	ADC_5		0x14           	// AN5	 
#define  	ADC_BIAS	0x3C           	// AN15	 

#define  VOL_KEY    0x80
#define  MENU_KEY   0x40
#define  UV_KEY     0x20
#define  LR_KEY     0x10
#define  FA_KEY     0x08
#define  PPT_KEY    0x01


#endif
