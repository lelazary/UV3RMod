#!/usr/bin/python
# uv3rFlash.py 
# a python program to assist with flashing the uv3r
# Copyright (C) 2013 Lior Elazary - lior hat elazary.com
#
#
# Derived from srecparser.py  Copyright (C) 2011 Gabriel Tremblay - initnull hat gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#Protocol
#   CMD            Param               Description
#   S                                  Enter ISP mode
#   C                                  Exit  ISP mode
#   I                                  Get Chip ID
#   F              HH                  Flash config register with hex HH
#   E                                  Erase flash memory
#   P              AAAA LL DDDD...     Program memory with starting at address AAAA length LL and data DDDDDD (2char hex code)
#   R              SSSS EEEE           Read memory from address SSSS to EEEE
#On success return OK
#On failure return ERR

"""
Motorola S-Record parser
- Kudos to Montreal CISSP Groupies
"""

import sys
import serial
import srecutils
import binascii
from optparse import OptionParser

def __generate_option_parser():
    usage_str = "usage: %prog [options]"
    parser = OptionParser(usage=usage_str)
    parser.add_option("-p", action="store", type="string",
        dest="port", help="Use the arduino on PORT [default: %default]", default="/dev/ttyUSB0")
    parser.add_option("-e", action="store_true",
                      dest="erase", help="Erase chip [default: %default]", default=False)
    parser.add_option("-d", action="store_true",
                      dest="debug", help="Show debug information [default: %default]", default=False)
    parser.add_option("-w", action="store", type="string",
                      dest="write", help="Write s-rec FILENAME to Flash memory ")
    parser.add_option("-c", action="store", type="string",
                      dest="config", help="Program CONFIG bits in hex")
    parser.add_option("-r", action="store", type="string",
                      dest="read", nargs=2, help="Read the flash memory from ADDR_BEGIN to ADDR_END in 4byte hex example C000 FFFF")
    parser.add_option("-a", action="store",
                      dest="auto", help="Erase, and then flash memory [default: %default]", default=False)
    parser.add_option("-v", action="store", type="string",
                      dest="verify", help="Verify that the filename VERIFY is in memory")

    return parser

def enterISP():
  serialPort.write("S"); #Enter ISP mode
  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    #if (data.startswith("OK
    data = serialPort.readline(36000);
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 

  return False

def enterFlashMode():
  serialPort.write("M"); #Enter ISP mode
  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    #if (data.startswith("OK
    data = serialPort.readline(36000);
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 

  return False

def exitISP():
  serialPort.write("C"); #exit ISP mode
  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    #if (data.startswith("OK
    data = serialPort.readline(36000);
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 

  return False

def sendErase():
  serialPort.write("E"); #Enter ISP mode
  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,100):
    data = serialPort.readline(36000);
    if options.debug: print data
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 
  return False

def eraseChip(serialPort):
  print "Erasing chip"
  if enterISP():
    print "OK"
  if sendErase():
    print "OK"
  else:
    print "Error"
  exitISP()

def sendConfigBits(bits):
  serialPort.write("F%s" % bits);

  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    data = serialPort.readline(36000);
    if options.debug: print data
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 
  return False

def configChip(bits):
  print "Config chip with %s " % bits
  if enterISP():
    print "OK"
  if sendConfigBits(bits):
    print "OK"
  else:
    print "Error"
  exitISP()

def sendRead(startAddr, endAddr):
  
  flashBuff = bytearray(endAddr-startAddr+1); #Array to store the return memory

  for i in xrange(0,endAddr-startAddr+1):
    flashBuff[i] = 0x00

  for addr in (xrange(startAddr,endAddr,256)):
    end = addr + 255
    cmd = "R%0.2X %0.2X" % (addr,end)
    if options.debug: print "Sending: %s" % cmd
    serialPort.write(cmd)
    gotData = False
    #given a 1 sec timeout, wait 10 secods
    for t in xrange(0,10):
      data = serialPort.readline(36000);
      if options.debug: print data
      if data.startswith("OK"):
        hData = data[3:len(data)-2]
        flashBuff[addr-startAddr:end-startAddr] = hData.decode('hex')
        break
      if data.startswith("ERR"):
        return False
        break

  return flashBuff

def read(startAddr, endAddr):
  print "Read chip from %s to %s" % (startAddr, endAddr)
  if enterISP():
    print "OK"
  if enterFlashMode():
    print "OK"

  flashBuff = sendRead(int(startAddr,16), int(endAddr,16))
  if (flashBuff):
    print "OK"
  else:
    print "Error"
  exitISP()

  return flashBuff

def sendProgram(addr, data_len, data):
  if options.debug: print 'Program Addr --%s--%s--%s--\n' % (addr, data_len, data)
  cmd = "P%s %s %s" % (addr, data_len, data)
  if options.debug: print 'send:', cmd
  serialPort.write(cmd); 
  sentData = False
  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    data = serialPort.readline(36000);
    if options.debug: print data
    if data.startswith("OK"):
      return int(data[3:],16)
      break
    elif data.startswith("ERR"):
      return False
      break

  return False

def sendBuffer(addr, buff):

  exitISP();
  #Program 128 bytes at a time
  if enterISP():
    print "OK"

  if enterFlashMode():
    print "OK"
  blockSize = 128 
  for i in xrange(0,len(buff),blockSize):
    ret = 0;
    #while(ret == 0) :
    ret = sendProgram(
          "%0.2X" % (addr+i),
          "%0.2X" % (blockSize),
          binascii.hexlify(buff[i:i+blockSize]))
    #print "%s" % binascii.hexlify(buff[i:i+128])

  exitISP()

def getFlashData(filename, startAddr):

  print "Write %s " % filename
  # open input file
  scn_file = open(filename)
  
  endAddr = 0xFFFF;
  programBuff = bytearray(endAddr-startAddr+1); #Array to store 16K of memory, and program it in one shot

  #Fill the byte array with default values (recommended ff so we dont exec anything in the chip inevitably)
  for i in xrange(0,endAddr-startAddr+1):
    programBuff[i] = 0x00

  linecount = 0
  for srec in scn_file:
      # Strip some file content
      srec = srec.strip("\n")
      srec = srec.strip("\r")
  
      # Validate checksum and parse record
      if not srecutils.validate_srec_checksum(srec):
          print "Invalid checksum found!"
      else:
          # Extract data from the srec
          record_type, data_len, addr, data, checksum = srecutils.parse_srec(srec)
  
          if record_type == 'S1':
              # Make a copy of the original data record for checksum calculation
              raw_data = data
  
              # Apply offset (default is 0)
              data = srecutils.offset_data(data, 0, 0, 0)
  
              # Get checksum of the new offset srec
              raw_offset_srec = ''.join([record_type, data_len, addr, raw_data])
              int_checksum = srecutils.compute_srec_checksum(raw_offset_srec)
              checksum = srecutils.int_to_padded_hex_byte(int_checksum)

              # output to file
              print ''.join([str(linecount), ':', data, '\n']),

  
              #data = ''.join([record_type, data_len, addr, data, checksum])
              plen = int(data_len,16) - 3 #Convert to a number and subtract the addr and length 
              decAddr = int(addr, 16)

              idx = decAddr-startAddr
              #Check the the address is in range
              if (idx < 0 or idx+plen > endAddr):
                print "ERR: Invalid address given %X" % decAddr
                break
              #Set the buffer with the program data
              print "Addr %X %X" % (decAddr, idx)
              #Convert the hex byte into in and store in our program memory
              for i in xrange(0,plen):
                programBuff[idx+i] = int(data[i*2:(i*2)+2],16)

          # All the other record types
          else:
              output_str = ' No Action '.join([srec, '\n'])
  
              output_str = ''.join([str(linecount), ': ', output_str])
  
              print output_str,
  
  
      # increment our fancy linecounter
      linecount += 1
  
  scn_file.close()

  return programBuff
if __name__ == "__main__":
    parser = __generate_option_parser()
    (options, args) = parser.parse_args(sys.argv)

    serialPort = serial.Serial(port = options.port, baudrate = 38400, timeout = 0.2)

    exitISP(); #Just incase we stopped before closing
    if options.erase:
      eraseChip(serialPort)
      #Send command to erase chip
    elif options.config is not None:
      configChip(options.config)
    elif options.auto:
      eraseChip(serialPort)
      configChip("00")
      print "Auto program"
    elif options.write:
      startAddr = 0xC000; #The start of the chip program memory
      programBuff = getFlashData(options.write, startAddr)
      if (programBuff):
        #Send the program to the chip
        sendBuffer(startAddr, programBuff)
    elif options.read:
      flashBuff = read(options.read[0], options.read[1])

      if (flashBuff):
        for i in xrange(0,len(flashBuff),16):
          print binascii.hexlify(flashBuff[i:i+16])
    elif options.verify:
      startAddr = 0xC000; #The start of the chip program memory
      programBuff = getFlashData(options.verify, startAddr)
      flashBuff = read("C000", "CFFF")

      for i in xrange(0,len(flashBuff),16):
        print "Chip:%s" % binascii.hexlify(flashBuff[i:i+16])
        print "File:%s" % binascii.hexlify(programBuff[i:i+16])
        print

    else:
      parser.print_help()
      sys.exit()


