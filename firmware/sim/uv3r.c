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
#include <uv3r.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <lcdSim.h>
#include <math.h>

int SEG14,SEG0,SEG1,R1IO,R16,R5IO,R5,R05,R00,R5PSR,R6IO,R06,R6,R6PSR,R7IO,R7,R7PSR,WTMR,WDTR,LCR,LBCR,R15,R13,R0IO,R0PSR,R0PU,R0,R1PSR,R2IO,R2PU,R2OD,R2,IENH,ADCRH,ADCM,ADSF,ADCRL,R24,R17,R11,ASIMR0,BRGCR0,ASISR0,IFRX0,IFTX0,RXBR,TXSR;
//
static unsigned char encoderState = 0;
unsigned char i;
unsigned char displayBuff[80];
unsigned char* LCD_ADDR = displayBuff;

GtkWidget* gtkArea;

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


  //Init interrupts
  IENH  = 0x0C;     //  x, INT0(6), INT1(5), INT2(4),RX(3),TX(2),x,x  // TX/RX enable 
  //IENM    = 0x80;     // T0E(7),T1E(6),T2E(5),T3E(4), -, -, -, ADCE(0) 
  //IENL    = 0x10;     // SPIE(7),BITE(6),WDTE(5),WTE(4),INT3(3),I2CE(2),x,x               
  
  RADIO_PW = 1; //Power on the radio
  SPK_EN = 0;  //Turn off the speaker


  GtkWidget *window;

  int argc = 0;
  char **argv  = NULL;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 380);
  gtk_window_set_title(GTK_WINDOW(window), "+-");

  GtkWidget* frame = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), frame);

  gtkArea = gtk_drawing_area_new();
  gtk_widget_set_usize(gtkArea,400,200);
  gtk_fixed_put(GTK_FIXED(frame), gtkArea, 0, 0);

  GtkWidget* button = gtk_button_new_with_label("m");
  gtk_widget_set_size_request(button, 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), button, 20, 220);

  gtk_widget_show_all(window);

  int i;
  for(i=0; i<80; i++)
    displayBuff[i] = 0;
  
}


//Voltage for keys
//Idel 250-255
//vol 0
//menu 53
//u/v 108
//f/a 155
//l/r 207
//

void updateLCDDisplay()
{
  int i;
  //Display LCD display
  for(i=0; i<12; i++)
  {
    if (i>5)
    {
      unsigned short mask = 0;
      mask |= ((displayBuff[(i-6)*4+2]&0xF0)>>4);
      mask |= ((displayBuff[(i-6)*4+3]&0xF0));
      mask |= ((displayBuff[(i-6)*4+4]&0xF0)<<4);
      mask |= ((displayBuff[(i-6)*4+5]&0xF0)<<8);
      displaySegment(gtkArea,  7+(i-6)*17, 3, 1, mask);
    } else {
      unsigned short mask = 0;
      mask |= ((displayBuff[i*4+2]&0x0F)<<0);
      mask |= ((displayBuff[i*4+3]&0x0F)<<4);
      mask |= ((displayBuff[i*4+4]&0x0F)<<8);
      mask |= ((displayBuff[i*4+5]&0x0F)<<12);
      displaySegment(gtkArea,  7+i*17, 33, 0, mask);
    }
  }

  gtk_main_iteration();
  //usleep(10000);
}

unsigned char getKeys()
{
  updateLCDDisplay();
  return 0;
}



unsigned short wDly_count;

//---------------------------------------------------------------
//	N ms delay 	by 4MHz crystal 	
//
//	(caution!) its only aprox because the loop is not accounted for
void msDelay(unsigned short value)
{
  unsigned short i;
  printf("Begin\n");
  for(i =0; i<value; i++)
  {
    updateLCDDisplay();
  }
  printf("end\n");


}

//---------------------------------------------------------------
//	N usec delay 	by 4MHz crystal 	
//
//	(caution!) It is available over 48us delay 
void delay(unsigned short value)		 
{
}


