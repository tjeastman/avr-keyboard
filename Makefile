CC = avr-gcc
OBJCOPY = avr-objcopy
DEVICE = atmega328p
PROGRAMMER = avrispv2
PORT = /dev/ttyACM0
F_CPU = 16000000L
BAUD = 9600
CFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -mmcu=$(DEVICE) -Os -std=c99 -I. -c

OBJECTS = main.o usart.o keyboard/base.o keyboard/protocol.o keyboard/state.o keyboard/scan.o keyboard/label.o

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

main.elf: $(OBJECTS)
	$(CC) -mmcu=$(DEVICE) -o $@ $(OBJECTS)

main.o: main.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: main.hex

flash: main.hex
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -qq -U flash:w:main.hex

clean:
	$(RM) main.hex main.elf $(OBJECTS)

.PHONY: all flash clean
