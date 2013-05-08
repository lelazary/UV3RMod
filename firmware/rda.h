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

#ifndef RDA_H
#define RDA_H

//Reg 1F
//GPIO0 css_out
//GPIO1 off VFM
//GPIO2 on  VHF
//GPIO3 off UHF
//GPIO4 on  VRX
//GPIO5 off TX-VCC
//GPIO6 SQ
//GPIO7 TX LED (red)

////First byte
//9 1001 //TX LED OFF sq output
//
////Second byte
//B 1011 5: Low  4: High  (Transmit power on)
//E 1110 5: HIGH 4: LOW   (Recive power on)
//A 1010 5: HIGH 4: HIGH   (All Off)
//
////Third Byte
//B 1011  3:Low 2: High  (VHF)
//E 1110  3:High 2: Low  (UHF)
//A 1010  3:Low 2: Low   (None)
//
////Forth Byte
//9 1001  VFM Off and css output
 
//CTCSS
//REg 0x45
//
//001 inner ctcss (TX ctcss)
//010 inner cdcss
//101 outter ctcss
//110 outter cdcss


struct RDAFormat {
  char address;
  unsigned short data;
};

void rda1846Init();

void rda1846GetStatus(short* rssi, short* vssi); //, short* dtmf, short* flags);
unsigned char rda1846GetDTMF();
void rda1846TXDTMF(unsigned char* values, unsigned int len, unsigned short delay);
void rda1846SetFreq(unsigned short freqU, unsigned short freqL);
void rda1846TX();
void rda1846RX(unsigned char useSq);
void rda1846SetPower(unsigned char power);
void rda1846SetGPIO(unsigned char gpio);
void rda1846SetReg(unsigned char addr, unsigned short data);
void rda1846TXDigital(unsigned char data, unsigned short t,
    unsigned short mark, unsigned short space);


#define TX_LED 0x80


#endif



