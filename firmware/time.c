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



#include "time.h"
#include <MC81F8816/MC81F8816.h>

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
