hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\lcd.lst" -c -o .\lcd.o .\lcd.c
hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\main.lst" -c -o .\main.o .\main.c
hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\rda.lst" -c -o .\rda.o .\rda.c
hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\spi.lst" -c -o .\spi.o .\spi.c
hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\time.lst" -c -o .\time.o .\time.c
hms800-cc -ramclr -mshort -Os -g -Wa,"-mhms800"  --option-file=C:\HMS800C\options\MC81F8816.opt -Wa,"-ahls=.\uv3r.lst" -c -o .\uv3r.o .\uv3r.c
hms800-cc -ramclr -mshort -Wl,"-Map=.\uv3r_firmware.map" --option-file=C:\HMS800C\options\MC81F8816.opt -o .\uv3r_firmware.out .\lcd.o .\main.o .\rda.o .\spi.o .\time.o .\uv3r.o 
hms800-elf-objcopy -O srec .\uv3r_firmware.out .\uv3r_firmware.hex
@ C:\HMS800C\bin\hold.exe
