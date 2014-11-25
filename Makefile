CC = avr-gcc
OBJCOPY = avr-objcopy
DEVICE = atmega328p
F_CPU = 16000000L
BAUD = 9600
CFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -mmcu=$(DEVICE) -Os -std=c99 -c

OBJECTS = main.o usart.o keyboard.o

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

main.elf: $(OBJECTS)
	$(CC) -mmcu=$(DEVICE) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: main.hex

flash: main.hex
	avrdude -p $(DEVICE) -c usbtiny -P usb -qq -U flash:w:main.hex

clean:
	$(RM) main.hex main.elf main.o usart.o keyboard.o

.PHONY: all flash clean
