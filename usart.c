#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>

#include "usart.h"

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
    UCSR0B = _BV(RXEN0) | _BV(TXEN0); // enable RX and TX
}

void my_putchar(char c) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
}

int my_putchar_printf(char c, FILE *stream) {
  my_putchar(c);
  return 0;
}

FILE uart_output = FDEV_SETUP_STREAM(my_putchar_printf, NULL, _FDEV_SETUP_WRITE);
