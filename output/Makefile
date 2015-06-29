CC = avr-gcc
OBJCOPY = avr-objcopy
DEVICE = atmega328p
F_CPU = 16000000L
BAUD = 9600
CFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -mmcu=$(DEVICE) -Os -std=c99 -I. -c

OBJECTS = usbdrv/usbdrvasm.o usbdrv/usbdrv.o main.o usart.o keyboard/protocol.o keyboard/state.o keyboard/scan.o keyboard/label.o

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

main.elf: $(OBJECTS)
	$(CC) -mmcu=$(DEVICE) -o $@ $(OBJECTS)

usbdrv/usbdrvasm.o: usbdrv/usbdrvasm.S
	$(CC) $(CFLAGS) -o usbdrv/usbdrvasm.o usbdrv/usbdrvasm.S

usbdrv/usbdrv.o: usbdrv/usbdrv.c usbdrv/usbconfig.h
	$(CC) $(CFLAGS) -o usbdrv/usbdrv.o usbdrv/usbdrv.c

main.o: main.c
	$(CC) $(CFLAGS) -Iusbdrv -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: main.hex

flash: main.hex
	avrdude -p $(DEVICE) -c usbtiny -P usb -qq -U flash:w:main.hex

clean:
	$(RM) main.hex main.elf $(OBJECTS)

.PHONY: all flash clean
