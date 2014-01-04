CC_WINDOWS_PATH="C:\\HMS800C"
CC_INSTALL_DIR=$(HOME)/.wine/drive_c/HMS800C
CC=wine $(CC_INSTALL_DIR)/bin/hms800-cc
OBJCOPY=wine $(CC_INSTALL_DIR)/bin/hms800-elf-objcopy
OBJSIZE=wine $(CC_INSTALL_DIR)/bin/hms800-elf-size
OBJDUMP=wine $(CC_INSTALL_DIR)/bin/hms800-elf-objdump
#TARGET=SIM

#OBJS=lcd.o rda.o spi.o time.o uv3r.o main.o Int.o uart.o ui.o serialCtrl.o util.o radioModes.o
OBJS=lcd.o rda.o main.o uart.o ui.o serialCtrl.o util.o radioModes.o

ifeq ($(TARGET),SIM)
	FOBJS=$(OBJS) sim/uv3r.o sim/spi.o
else
	FOBJS=$(OBJS) Int.o uv3r.o spi.o
endif


all: uv3r_firmware

uv3r_firmware: $(FOBJS)
	echo "Linking..."
ifeq ($(TARGET),SIM)
	gcc -g -o uv3r_firmware $(FOBJS) `pkg-config --libs --cflags gtk+-2.0` -lgthread-2.0
else
	$(CC) -ramclr -mshort -Werror -Wl,"-Map=uv3r_firmware.map" --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -o uv3r_firmware.out $(FOBJS) 
	$(OBJCOPY) -O srec uv3r_firmware.out uv3r_firmware.hex
	$(OBJSIZE) uv3r_firmware.out 
endif

%.o: %.c sim/lcdSim.h
ifeq ($(TARGET),SIM)
		gcc -g -Isim -I./ `pkg-config --libs --cflags gtk+-2.0` -c -o $*.o $<
else
	$(CC) -ramclr -mshort -Os -g -Werror -Wa,"-mhms800"  --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -Wa,"-ahls=$*.lst" -c -o $*.o $<
endif

%.o: %.s
ifneq ($(TARGET),SIM)
	$(CC) -ramclr -mshort -Os -g -Werror -Wa,"-mhms800"  --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -Wa,"-ahls=$*.lst" -c -o $*.o $<
endif

flash: uv3r_firmware
	../programmer/uv3rFlash.py -d -a uv3r_firmware.hex

dump: uv3r_firmware
	$(OBJDUMP) -D -S uv3r_firmware.out | less > uv3r_firmware.dis

clean:
	rm uv3r_firmware.hex *.o *.lst sim/*.o

