#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile unsigned char *keyboard_buffer;
volatile int keyboard_buffer_pos;

void keyboard_init(void) {
  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;

  keyboard_buffer = (unsigned char *)malloc(10 * sizeof(unsigned char));
  keyboard_buffer_pos = 0;
}

int get_buffer_size(void) {
  return keyboard_buffer_pos;
}

unsigned char get_buffer_item(int i) {
  return keyboard_buffer[i];
}

void reset_buffer(void) {
  keyboard_buffer_pos = 0;
}

void keyboard_interrupt(void) {
  static scan_state state = SCAN_START;
  static scan_code c;

  switch (state) {
  case SCAN_START:
    state = SCAN_SCAN;
    c.value = 0;
    c.nbits = 0;
    c.parity = 0;
    break;
  case SCAN_SCAN:
    c.value = c.value >> 1;
    if (PIND & (1<<PD3))
      c.value |= 0x80;
    c.nbits += 1;
    if (c.nbits == 8)
      state = SCAN_PARITY;
    break;
  case SCAN_PARITY:
    state = SCAN_END;
    c.parity = PIND & (1<<PD3) ? 1 : 0;
    break;
  case SCAN_END:
    state = SCAN_START;
    keyboard_buffer[keyboard_buffer_pos] = c.value;
    keyboard_buffer_pos += 1;
    break;
  default:
    break;
  }
}
