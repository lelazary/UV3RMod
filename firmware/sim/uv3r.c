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
unsigned char i;
unsigned char displayBuff[80];
unsigned char* LCD_ADDR = displayBuff;

GtkWidget* gtkArea;
GtkWidget* buttons[8];

unsigned char buttonsState = 0;
unsigned char encoderState = 0;

// Read the dial encoder using gray code to avoid debouncing. 
//Insperations from
// http://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino

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

void buttonPress(GtkWidget *widget, gpointer data)
{
  buttonsState |= GPOINTER_TO_INT(data);
}

void encoderPress(GtkWidget *widget, gpointer data)
{
  if (GPOINTER_TO_INT(data))
  {
    encoderState = +1;
  } else {
    encoderState = -1;
  }
}


void initIOPorts()
{
  int i;
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

  buttons[0] = gtk_button_new_with_label("LR");
  gtk_widget_set_size_request(buttons[0], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[0], 30, 200);
  g_signal_connect(buttons[0], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(LR_KEY));

  buttons[1] = gtk_button_new_with_label("FA");
  gtk_widget_set_size_request(buttons[1], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[1], 30, 240);
  g_signal_connect(buttons[1], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(FA_KEY));

  buttons[2] = gtk_button_new_with_label("UV");
  gtk_widget_set_size_request(buttons[2], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[2], 120, 240);
  g_signal_connect(buttons[2], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(UV_KEY));

  buttons[3] = gtk_button_new_with_label("M");
  gtk_widget_set_size_request(buttons[3], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[3], 160, 240);
  g_signal_connect(buttons[3], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(MENU_KEY));

  buttons[4] = gtk_button_new_with_label("V");
  gtk_widget_set_size_request(buttons[4], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[4], 200, 240);
  g_signal_connect(buttons[4], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(VOL_KEY));

  buttons[5] = gtk_button_new_with_label("PTT");
  gtk_widget_set_size_request(buttons[5], 100, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[5], 20, 280);
  g_signal_connect(buttons[5], "clicked", G_CALLBACK(buttonPress), GINT_TO_POINTER(PTT_KEY));


  buttons[6] = gtk_button_new_with_label("+");
  gtk_widget_set_size_request(buttons[6], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[6], 120, 200);
  g_signal_connect(buttons[6], "clicked", G_CALLBACK(encoderPress), GINT_TO_POINTER(1));

  buttons[7] = gtk_button_new_with_label("-");
  gtk_widget_set_size_request(buttons[7], 35, 35);
  gtk_fixed_put(GTK_FIXED(frame), buttons[7], 155, 200);
  g_signal_connect(buttons[7], "clicked", G_CALLBACK(encoderPress), GINT_TO_POINTER(0));

  gtk_widget_show_all(window);

  for(i=0; i<6; i++)
  {
  }
  

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

  gtk_main_iteration_do(FALSE);
  usleep(1000);
}

unsigned char getKeys()
{
  unsigned char b= buttonsState;
  buttonsState = 0;
  return b;
}

char getDialEncoder()
{
  char s = encoderState;
  encoderState = 0;
  updateLCDDisplay();
  return s;
}



unsigned short wDly_count;

//---------------------------------------------------------------
//	N ms delay 	by 4MHz crystal 	
//
//	(caution!) its only aprox because the loop is not accounted for
void msDelay(unsigned short value)
{
  unsigned short i;
  for(i =0; i<value/10; i++)
  {
    updateLCDDisplay();
  }
}

//---------------------------------------------------------------
//	N usec delay 	by 4MHz crystal 	
//
//	(caution!) It is available over 48us delay 
void delay(unsigned short value)		 
{
}


