#!/usr/bin/python
# radioUI.py 
# Graphical UI to use interface with the radio
# Copyright (C) 2013 Lior Elazary - lior hat elazary.com
#
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

import sys
import serial
import binascii
import struct 
from optparse import OptionParser
import pygtk
pygtk.require('2.0')
import gtk
import re

currentFreq = 000.000
serialPort = serial.Serial(port = "/dev/ttyUSB0", baudrate = 9600, timeout = 0.2)

def sendDTMF(time):
  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));
  serialPort.write("d");
  serialPort.write(chr(time>>8 & 0xff)); #//chr(0));
  serialPort.write(chr(time & 0xff));
  serialPort.write("\r\n");

def sendTX():
  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));
  serialPort.write("T");
  serialPort.write("\r\n");

def sendRX():
  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));
  serialPort.write("R");
  serialPort.write("\r\n");

  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));
  serialPort.write("R");
  serialPort.write("\r\n");

  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));
  serialPort.write("R");
  serialPort.write("\r\n");

def sendFreq(freqM, freqK):
  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));

  serialPort.write("F");
  serialPort.write(chr(freqM>>8 & 0xff)); #//chr(0));
  serialPort.write(chr(freqM & 0xff));
  serialPort.write(chr(freqK>>8 & 0xff)); #//chr(0));
  serialPort.write(chr(freqK & 0xff));
  serialPort.write("\r\n");

  #given a 1 sec timeout, wait 10 secods
  for t in xrange(0,10):
    #if (data.startswith("OK
    data = serialPort.readline(36000);
    print data
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 

  return True

def sendRDA(addr, val):
  print "Send to rda %X %X" % (addr, val)
  serialPort.write(chr(0xAA));
  serialPort.write(chr(0x55));

  serialPort.write("d"); #S
  serialPort.write(chr(addr)); #//chr(0));
  serialPort.write(chr(val>>8 & 0xff)); #//chr(0));
  serialPort.write(chr(val & 0xff));
  serialPort.write("\r\n");

  for t in xrange(0,10):
    #if (data.startswith("OK
    data = serialPort.readline(36000);
    print data
    if data.startswith("OK"):
      return True
    if data.startswith("ERR"):
      return False 

  return True

class RadioGui:
    def __init__(self):
        numeric_const_pattern = r"""
        [-+]? # optional sign
        (?:
            (?: \d* \. \d+ ) # .1 .12 .123 etc 9.1 etc 98.1 etc
            |
            (?: \d+ \.? ) # 1. 12. 123. etc 1 12 123 etc
        )
        # followed by optional exponent part if desired
        (?: [Ee] [+-]? \d+ ) ?
        """
        self.rx = re.compile(numeric_const_pattern, re.VERBOSE)

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.set_size_request(200, 200)
        self.window.set_title("Radio GUI Interface")
        self.window.connect("delete_event", self.delete_event)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)

        self.vbox = gtk.VBox(False, 0)
        self.window.add(self.vbox)
        self.vbox.show()

        self.freqLabel = gtk.Label("Current Freq: %s MHz" % currentFreq)
        self.vbox.pack_start(self.freqLabel, True, True, 0)
        self.freqLabel.show()

        self.entry = gtk.Entry()
        self.entry.set_max_length(50)
        self.entry.connect("activate", self.setFreq, self.entry)
        self.entry.connect("changed", self.checkSetFreq, self.entry)
        self.entry.set_text("145.555")
        self.vbox.pack_start(self.entry, True, True, 0)
        self.entry.show()

        self.entry2 = gtk.Entry()
        self.entry2.set_max_length(50)
        self.vbox.pack_start(self.entry2, True, True, 0)
        self.entry2.show()

        self.entry3 = gtk.Entry()
        self.entry3.set_max_length(50)
        self.entry3.connect("activate", self.setRDA, self.entry3)
        self.vbox.pack_start(self.entry3, True, True, 0)
        self.entry3.show()

        self.button = gtk.Button("TX")
        self.button.connect("button_press_event", self.startTX, None)
        self.button.connect("button_release_event", self.stopTX, None)
        self.vbox.pack_start(self.button, True, True, 0)
        self.button.show()

        self.button2 = gtk.Button("DTMF")
        self.button2.connect("button_press_event", self.sendDTMF, None)
        self.vbox.pack_start(self.button2, True, True, 0)
        self.button2.show()


        self.window.show()

    def delete_event(self, widget, event, data=None):
      print "Turn TX off, just incase"
      return False
    def destroy(self, widget, data=None):
      gtk.main_quit()

    def startTX(self, widget, event, data=None):
      print "Transmiting..."
      sendTX();
    def stopTX(self, widget, event, data=None):
      print "Stop Transmiting."
      sendRX();
    def sendDTMF(self, widget, event, data=None):
      print "send DTMF."
      sendDTMF(int(self.entry2.get_text(), 10))

    def setFreq(self, widget, entry):
      currentFreq = self.entry.get_text()
      self.freqLabel.set_label("Current Freq: %s MHz" % currentFreq)
      idx = currentFreq.find('.');
      freqM = int(currentFreq[0:idx]);
      freqK = int(currentFreq[idx+1:len(currentFreq)])
      print "set freq to : %i MHz %i KHz\n" % (freqM, freqK)
      sendFreq(freqM, freqK);

    def checkSetFreq(self, widget, entry):
      freq = self.entry.get_text()
      newtext = self.rx.findall(freq)
      if len(newtext) :
        entry.set_text(newtext[0])
      else:
        entry.set_text("")

    def setRDA(self, widget, entry):
      sendRDA(int(self.entry2.get_text(), 16), int(self.entry3.get_text(),16));

    def main(self):
        gtk.main()



def packIntegerAsUShort(value):
    """Packs a python 4 byte unsigned integer to an arduino unsigned long"""
    return struct.pack('I', value)    #should check bounds

if __name__ == "__main__":
  base = RadioGui()
  base.main()

