
CC_WINDOWS_PATH="C:\\HMS800C"
CC_INSTALL_DIR=$(HOME)/.wine/drive_c/HMS800C
CC=wine $(CC_INSTALL_DIR)/bin/hms800-cc
OBJCOPY=wine $(CC_INSTALL_DIR)/bin/hms800-elf-objcopy
OBJSIZE=wine $(CC_INSTALL_DIR)/bin/hms800-elf-size

OBJS=lcd.o rda.o spi.o time.o uv3r.o main.o Int.o uart.o ui.o serialCtrl.o

all: uv3r_firmware

uv3r_firmware: $(OBJS)
	$(CC) -ramclr -mshort -Werror -Wl,"-Map=uv3r_firmware.map" --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -o uv3r_firmware.out $(OBJS) 
	$(OBJCOPY) -O srec uv3r_firmware.out uv3r_firmware.hex
	$(OBJSIZE) uv3r_firmware.out 

%.o: %.c
	$(CC) -ramclr -mshort -Os -g -Werror -Wa,"-mhms800"  --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -Wa,"-ahls=$*.lst" -c -o $*.o $<

%.o: %.s
	$(CC) -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file="$(CC_WINDOWS_PATH)\\options\\MC81F8816.opt" -Wa,"-ahls=$*.lst" -c -o $*.o $<

flash: uv3r_firmware
	../programmer/uv3rFlash.py -d -a uv3r_firmware.hex

clean:
	rm uv3r_firmware.hex *.o *.lst

