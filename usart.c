#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>

#include "usart.h"

void usart_init(void)
{
    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;

#if USE_2X
    UCSR1A |= _BV(U2X1);
#else
    UCSR1A &= ~(_BV(U2X1));
#endif

    UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // 8-bit data
    UCSR1B = _BV(RXEN1) | _BV(TXEN1); // enable RX and TX
}

void usart_putchar(char c)
{
  loop_until_bit_is_set(UCSR1A, UDRE1);
  UDR1 = c;
}

FILE usart_output = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);
