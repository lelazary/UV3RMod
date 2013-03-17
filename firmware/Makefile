
CC=wine /home/elazary/.wine/drive_c/HMS800C/bin/hms800-cc
OBJCOPY=wine /home/elazary/.wine/drive_c/HMS800C/bin/hms800-elf-objcopy

OBJS=lcd.o rda.o spi.o time.o uv3r.o main.o

all: uv3r_firmware

uv3r_firmware: $(OBJS)
	$(CC) -ramclr -mshort -Wl,"-Map=uv3r_firmware.map" --option-file="C:\\HMS800C\\options\\MC81F8816.opt" -o uv3r_firmware.out $(OBJS) 
	$(OBJCOPY) -O srec uv3r_firmware.out uv3r_firmware.hex

%.o: %.c
	$(CC) -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file="C:\\HMS800C\\options\\MC81F8816.opt" -Wa,"-ahls=lcd.lst" -c -o $*.o $<

flash: uv3r_firmware
	../programmer/uv3rFlash.py -d -a uv3r_firmware.hex

clean:
	rm uv3r_firmware.hex *.o

