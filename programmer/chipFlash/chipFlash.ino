/* 
 * This program is used to flash the MC81F8616L chip which is part of the uv3r ham radio
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

#include <avr/io.h> 

int CLK = 7;
int SIO = 6;
int VPP = 5;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(CLK, OUTPUT);   
  pinMode(SIO, OUTPUT);   
  pinMode(VPP, OUTPUT);   
  digitalWrite(CLK, LOW);
  digitalWrite(VPP, HIGH);
   
  Serial.begin(9600);  
}

unsigned char flashData[128];

unsigned char waitForData()
{
  unsigned char gotData = 0;
  int i;
  pinMode(SIO, INPUT);
  
  for(i=0; i<3000; i++)
  {
    if (!digitalRead(SIO)) //Wait until we get a response
    {
      break;
      gotData = 1;
    }
    delayMicroseconds(100);
  }  
  
  return gotData;
}

void spiTX(unsigned char data)
{
  pinMode(SIO, OUTPUT);
  int counter;
  for(counter=8; counter  ; counter--)
  {
    //Place the data on the line
    if (data & 0x01)
      digitalWrite(SIO, HIGH);
    else
      digitalWrite(SIO, LOW);
      
    //Clock the data
    digitalWrite(CLK, LOW);
    delayMicroseconds(4);
  
    //Set data/read
    digitalWrite(CLK, HIGH);
    delayMicroseconds(8);
    
    data >>= 1; //next bit
  }
  
}

unsigned char spiRX()
{
  unsigned char data;
  pinMode(SIO, INPUT);
  int counter;
  for(counter=0; counter<8  ; counter++)
  {
    data >>= 1;
    digitalWrite(CLK, LOW);
    delayMicroseconds(4);
    //Set data/read
    digitalWrite(CLK, HIGH);
    if (digitalRead(SIO)) 
      data |= 0x80;
    delayMicroseconds(8);
       
  }
  pinMode(SIO, OUTPUT);
  digitalWrite(SIO, HIGH);
  return data;
}

void enterISP()
{
  //Enter 
  digitalWrite(CLK, HIGH);
  digitalWrite(SIO, HIGH);
  digitalWrite(VPP, LOW);
  delay(240); 
}

void exitISP()
{
  delay(200);
  digitalWrite(CLK, LOW);
  digitalWrite(SIO, LOW);
  digitalWrite(VPP, HIGH);  
}

void sendData(unsigned char* data, int length)
{
  int i=0; 
  for(i=0; i<length; i++)
  {
    spiTX(data[i]);  //10101010
    delayMicroseconds(100);
  }
}

unsigned char getChipID()
{
  
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x01, 0x00, 0x00, 0x05 , 0x00};
  sendData(data, 9);

  delay(1); //waitForData();

  unsigned char val = spiRX();
  Serial.print("Chip id: ");
  Serial.println(val, HEX);

  if (val == 0x82)
    return 1; //Success
  else
    return 0; //Fail to get ID
  
}

void setMode()
{
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x01, 0x00, 0x00, 0x05 , 0x08};
  sendData(data, 9);

  //Read chip ID
  delay(1); //waitForData();
  unsigned char val = spiRX();
  Serial.print("Mode: ");
  Serial.println(val, HEX);
}

void setRegister()
{
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x02, 0x00, 0x00, 0x03 , 0x05, 
   0x00}; //Register 20FF value to 0x00
  sendData(data, 10);

  //Read chip ID
  delay(1); //waitForData();
  unsigned char val = spiRX();
  Serial.print("register: ");
  Serial.println(val, HEX);
}

void setFlashAddr(unsigned short addr, unsigned char len)
{
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x80, 0xC0, 0x00, 0x00 };
  
   data[4] = len;
   data[5] = (addr >> 8) & 0xFF;
   data[6] = addr & 0xFF;
   
  sendData(data, 8);


}

void setEraseMode()
{
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x01, 0x00, 0x00, 0x03 , 0x01};
  sendData(data, 9);

  //Read chip ID
  delay(1); //waitForData();
  unsigned char val = spiRX();
  Serial.print("Erase Mode: ");
  Serial.println(val, HEX);
  
}

void eraseFlash()
{
  enterISP();
  if (!getChipID())
  {
    Serial.println("Failed to get chip ID");
    return;
  }
  delay(30);
  setEraseMode();
  
  waitForData();
  
  delay(2000);
  setRegister();
  delay(30);
  
  exitISP();
  
  Serial.println("Finished Erasing");
  
  
}

void setFastClock()
{
 
 DDRD = 0xFF;
  while(1) {
    PORTD ^= 0xFF;
  } 
}

void programFuse()
{
  enterISP();
  if (!getChipID())
  {
    Serial.println("Failed to get chip ID");
    return;
  }
  delay(30);
  setMode();
  delay(30);
  setRegister();
  delay(30);
  
  exitISP();
}

void programFlash(unsigned short addr, unsigned char len)
{
  enterISP();
  if (!getChipID())
  {
    Serial.println("Failed to get chip ID");
    return;
  }
  delay(30);
  setMode();
  delay(30);
  
  setFlashAddr(addr, len);
  
  
  int i=0; 
  for(i=0; i<len; i++)
  {
    spiTX(flashData[i]);  
    delayMicroseconds(100);
  }
  
    //Read chip ID
  delay(1); //waitForData();
  unsigned char val = spiRX();
  Serial.print("Program result: ");
  Serial.println(val, HEX);
  
  exitISP();
}

void setReadRange(unsigned short startAddr, unsigned short endAddr)
{
  unsigned char data[] =
  { 0x55, 0xAA, 0x5A, 0xA5, 0x05, 0x00, 0x00, 0x04 ,
   0xC0, 0x00,
   0xC0, 0xFF, 0x00};
   
   data[8] = (startAddr >> 8) & 0xFF;
   data[9] = startAddr & 0xFF;
   data[10] = (endAddr >> 8) & 0xFF;
   data[11] = endAddr & 0xFF;
   sendData(data, 13);
   
}


void readFlash(unsigned short startAddr, unsigned short endAddr)
{
  enterISP();
  int i=0; 
  if (!getChipID())
  {
    Serial.println("Failed to get chip ID");
    return;
  }
  delay(30);
  setMode();
  delay(30);
  
  setReadRange(startAddr, endAddr);
  
  waitForData();
  
  unsigned short len = endAddr - startAddr;
  
  unsigned char buffer[len];
  for(i=0; i<len; i++)
    buffer[i] = spiRX();
  
  setMode();
  delay(30);
  
  exitISP();
 
  Serial.println("Data: "); 
  for(i=0; i<len; i++)
  {
     Serial.print(buffer[i], HEX);
     Serial.print(" ");
  }
  Serial.println();
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'A'+10);
}

void loop() {
  int i;

  if (Serial.available() > 0)
   {
     unsigned char d = Serial.read();
     switch (d)
     {
       case 'F': 
       {
           Serial.println("Config Device");
           programFuse();
           break;
       }
       case 'E':
       {
          Serial.println("Erasing");
          eraseFlash();
          break;
       }
       case 'R':
       {
          char strData[9];
          i = 0;
          while(i < 9)
            if (Serial.available() > 0)
              strData[i++] = Serial.read();
          unsigned short startAddr = 
             (getVal(strData[0]) << 12) |
             (getVal(strData[1]) << 8)  |
             (getVal(strData[2]) << 4)  |
              getVal(strData[3]);

          unsigned short endAddr = 
             (getVal(strData[5]) << 12) |
             (getVal(strData[6]) << 8)  |
             (getVal(strData[7]) << 4)  |
              getVal(strData[8]);
          Serial.println("Reading");
          Serial.print(startAddr, HEX);
          Serial.print(" ");
          Serial.println(endAddr, HEX);
          readFlash(startAddr, endAddr);
          break;
       }
       case 'P':
       {
          char strData[4];
          i = 0;
          while(i < 4)
            if (Serial.available() > 0)
              strData[i++] = Serial.read();
          unsigned short addr = 
             (getVal(strData[0]) << 12) |
             (getVal(strData[1]) << 8)  |
             (getVal(strData[2]) << 4)  |
              getVal(strData[3]);
              
         Serial.read(); //Skip the space
         unsigned char gotData = 0;
         unsigned char len = 0;
         while(!gotData)
         {
           if (Serial.available() > 2)
           {
             
             for(i=0; i<3; i++)
              strData[i] = Serial.read();
             if (strData[1] == '#') //End of data
             {
               gotData = 1;
               break;
             }
             else 
             {
               flashData[len++] = 
                 (getVal(strData[1]) << 4)  |
                  getVal(strData[2]);
             }
             if (len > 128) break; //only up to 128 char
           } else {
            // break;
           }
         }

         if (len > 0)
         {
             Serial.println("Program Flash at: ");
             Serial.print(addr, HEX);
             Serial.print(" Len: ");
             Serial.println(len, HEX);
             programFlash(addr, len); 
         } else {
             Serial.println("Can not flash with no data");
         }
         break;
       }
       case 'I' :
       {
         enterISP();
         break;
       }
       case 'C' :
       {
         exitISP();
         break;
       }
     }
  }
}

