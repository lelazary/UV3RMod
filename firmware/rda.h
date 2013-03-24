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


struct RDAFormat {
  char address;
  unsigned short data;
};


unsigned short rda1846GPIO = 0x0000;

CODE struct RDAFormat RDAinit[30] = {
 { 0x30, 0x0001 }, 
 { 0x30, 0x0004 },
 { 0x04, 0x0FD0 },
 { 0x0B, 0x1A10 },
 { 0x2B, 0x32C8 },
 { 0x2C, 0x1964 },
 { 0x32, 0x627C },
 { 0x33, 0x0AF2 },
 { 0x47, 0x2C2F },
 { 0x4E, 0x293A }, 
 { 0x54, 0x1D4C },
 { 0x56, 0x0652 }, 
 { 0x6E, 0x062D },
 { 0x70, 0x1029 },
 { 0x7F, 0x0001 },
 { 0x05, 0x001F },
 { 0x7F, 0x0000 },
 { 0x30, 0x3006 },
 
 { 0x0A, 0x0400 },        //PA Bias 0000 0100 '00 00' 0000
//{ 0x1F, 0x1EB9 },        //GPIO selection 0001 1110 1011 1001
 { 0x1F, 0x0000 },        //GPIO selection 0001 1110 1011 1001

//GPIO0 css_out
//GPIO1 off VFM
//GPIO2 on  VHF
//GPIO3 off UHF
//GPIO4 on  VRX
//GPIO5 off TX-VCC
//GPIO6 SQ
//GPIO7 TX LED (red)

//
{ 0x30, 0x3006 }, 
{ 0x0F, 0x6be4 },
{ 0x29, 0x0011 }, //145.525
{ 0x2A, 0xC3A8 }, //145.525
//{ 0x29, 0x0013 }, //NOAA 162.550
//{ 0x2A, 0xd7b0 }, //NOAA 162.550
{ 0x48, 0x03FF }, //0000 0011 1111 0000
{ 0x49, 0x01b3 }, //0003
{ 0x3C, 0x0958 }, //0000 1001 0101 1000 
{ 0x43, 0x1F1F },
{ 0x30, 0x3006 },
{ 0x36, 0x1000 },

};

//Freq * 4096
CODE short dtmfTone[10][2] = {
 { 5472, 3854 }, //0 
 { 4952, 2855 }, //1 
 { 5472, 2855 }, //2 
 { 6050, 2855 }, //3 
 { 4952, 3154 }, //4 
 { 5472, 3154 }, //5 
 { 6050, 3154 }, //6 
 { 4952, 3490 }, //7 
 { 5472, 3490 }, //8 
 { 6050, 3490 }, //9 
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


#define TX_LED 0x80


#endif



