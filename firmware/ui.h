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

//User interface related functions

#ifndef UI_H
#define UI_H


enum DISPLAY_MODE {
  FREQ_DISPLAY,
  STATUS_DISPLAY,
  DTMF_DISPLAY,
  MAX_DISPLAY_MODE}; //Last one used to know the size of the enum


void showFreqDisplayMode(unsigned char showTX);
void showStatusDisplayMode();
void showDTMFDisplayMode();
void updateDisplay(unsigned char mode);
void updateNum(unsigned short* num, unsigned char digit, char encDir);

  //Convert from DCB to binary of freqM and freqK while using short (2 byte) numbers
void updateRDA1846Freq(unsigned short freqM, unsigned short freqK);

extern unsigned char changeMode;
extern unsigned char displayMode;

#endif
