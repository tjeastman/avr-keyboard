#include <avr/io.h>
#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"

typedef enum {
  KEYBOARD_START = 0,
  KEYBOARD_SCAN = 1,
  KEYBOARD_PARITY = 2,
  KEYBOARD_END = 3,
} kstate;

unsigned char keyboard_buffer[100];
int keyboard_buffer_pos = 0;

ISR(INT0_vect) {
  static kstate keyboard_state = KEYBOARD_START;
  static int keyboard_bits_read = 0;
  static unsigned char keyboard_value = 0;
  if (keyboard_state == KEYBOARD_START) {
    keyboard_state = KEYBOARD_SCAN;
  } else if (keyboard_state == KEYBOARD_SCAN) {
    keyboard_value = keyboard_value >> 1;
    if (PIND & (1<<PD3))
      keyboard_value |= 0x80;
    keyboard_bits_read += 1;
    if (keyboard_bits_read == 8) {
      keyboard_bits_read = 0;
      keyboard_state = KEYBOARD_PARITY;
    }
  } else if (keyboard_state == KEYBOARD_PARITY) {
    keyboard_state = KEYBOARD_END;
  } else {
    keyboard_state = KEYBOARD_START;
    keyboard_buffer[keyboard_buffer_pos] = keyboard_value;
    keyboard_value = 0;
    keyboard_buffer_pos += 1;
  }
}

int main(void) {
  int i;
  uart_init();
  stdout = &uart_output;

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;

  // enable interrupts
  sei();

  while (1) {
    for (i = 0; i < keyboard_buffer_pos; ++i) {
      printf("0x%x [%d] ", keyboard_buffer[i], keyboard_buffer[i]);
    }
    if (keyboard_buffer_pos > 0) {
      printf("\r\n");
    }
    keyboard_buffer_pos = 0;
    _delay_ms(4000);
  }

  return 0;
}
