CC = avr-gcc
OBJCOPY = avr-objcopy
DEVICE = atmega328p
F_CPU = 16000000L
BAUD = 9600
CFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -mmcu=$(DEVICE) -Os -c

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.elf: %.o
	$(CC) -mmcu=$(DEVICE) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: main.hex

flash: main.hex
	avrdude -p $(DEVICE) -c usbtiny -P usb -U flash:w:main.hex

clean:
	$(RM) main.hex main.elf main.o

.PHONY: all flash clean
