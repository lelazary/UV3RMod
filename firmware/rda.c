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

#include "rda.h"
#include "spi.h"
#include "uv3r.h"

void initRDA1846()
{
  RDA1846_SEN = 1;
  RDA1846_SCK = 1;
  for(i=0; i<30; i++)
  {
    SPITransfer(RDAinit[i].address, RDAinit[i].data);
  }
}
