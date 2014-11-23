#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include <util/setbaud.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

void my_putnumber(unsigned int n) {
  int i;
  char buf[64];
  sprintf(buf, "0x%x [%d] ", n, n);
  for (i = 0; i < strlen(buf); ++i)
    my_putchar(buf[i]);
}

int keyboard_state = 0;
unsigned char keyboard_value = 0;
int keyboard_tmp = 0;
int keyboard_valid = 0;
unsigned char keyboard_buffer[2000];
int keyboard_buffer_pos = 0;

ISR(INT0_vect) {
  if (keyboard_state == 0) {
    keyboard_state = 1;
    keyboard_valid = 0;
  } else if (keyboard_state > 0 && keyboard_state <= 8) {
    keyboard_tmp = PIND & _BV(PD3);
    if (keyboard_tmp) {
      keyboard_value = keyboard_value >> 1;
      keyboard_value |= 0x80;
    } else {
      keyboard_value = keyboard_value >> 1;
    }
    keyboard_state += 1;
  } else if (keyboard_state == 9) {
    keyboard_state += 1;
  } else {
    keyboard_state = 0;
    keyboard_valid = 1;
    keyboard_buffer[keyboard_buffer_pos] = keyboard_value;
    keyboard_value = 0;
    keyboard_buffer_pos += 1;
  }
}

int main(void) {
  int i;
  uart_init();

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;

  // enable interrupts
  sei();

  while (1) {
    for (i = 0; i < keyboard_buffer_pos; ++i) {
      my_putnumber((unsigned int)keyboard_buffer[i]);
    }
    if (keyboard_buffer_pos > 0) {
      my_putchar('\r');
      my_putchar('\n');
    }
    keyboard_buffer_pos = 0;
    _delay_ms(4000);
  }

  return 0;
}
