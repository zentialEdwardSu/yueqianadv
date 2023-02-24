# VPATH = include

MODE=RELEASE

ifeq ($(MODE), DEBUG)
CFLAGS += -DEBUG
endif

HEADER_PATH = -I./include/
LIB_PATH = -L./lib/

LIBS = ./lib/libjpeg.a

# --------build lcd info---------
lcd_info.elf: lcd_info.o
	arm-linux-gcc -o lcd_info.elf lcd_info.o

lcd_info.o: lcd_info.c
	arm-linux-gcc -c lcd_info.c


# --------build lcd -------------
lcd.elf: lcd.o LCDutils.o
	arm-linux-gcc -o lcd.elf lcd.o LCDutils.o

lcd.o: lcd.c
	arm-linux-gcc -c $(CFLAGS)  lcd.c $(HEADER_PATH)

LCDutils.o: LCDutils.c
	arm-linux-gcc -c $(CFLAGS)  LCDutils.c $(HEADER_PATH)

# ---------- build lcd_img ---------
lcd_img.elf:lcd_img.o LCDutils.o LCDjpeg.o
	arm-linux-gcc -o lcd_img.elf lcd_img.o LCDutils.o LCDjpeg.o $(LIB_PATH) $(LIBS)

lcd_img.o: lcd_img.c 
	arm-linux-gcc -c $(CFLAGS) lcd_img.c  $(HEADER_PATH)

LCDjpeg.o: LCDjpeg.c
	arm-linux-gcc -c $(CFLAGS) LCDjpeg.c $(HEADER_PATH)

# --------- build commandline -------
Bettercommandline.elf: Bettercommandline.o LCDutils.o
	arm-linux-gcc -o Bettercommandline.elf Bettercommandline.o LCDutils.o

Bettercommandline.o: Bettercommandline.c
	arm-linux-gcc -c Bettercommandline.c $(HEADER_PATH)

# -------- build adv -------
adv.elf:adv.o LCDutils.o LCDjpeg.o
	arm-linux-gcc -o adv.elf adv.o LCDutils.o LCDjpeg.o $(LIB_PATH) $(LIBS)
adv.0: adv.c
	arm-linux-gcc -c $(CFLAGS) adv.c $(HEADER_PATH)

.PHONY:clean
clean:
	rm *.elf *.o