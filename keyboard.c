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
  static kstate keyboard_state = KEYBOARD_START;
  static int keyboard_bits_read = 0;
  static unsigned char keyboard_value = 0;
  switch (keyboard_state) {
  case KEYBOARD_START:
    keyboard_state = KEYBOARD_SCAN;
    break;
  case KEYBOARD_SCAN:
    keyboard_value = keyboard_value >> 1;
    if (PIND & (1<<PD3))
      keyboard_value |= 0x80;
    keyboard_bits_read += 1;
    if (keyboard_bits_read == 8) {
      keyboard_bits_read = 0;
      keyboard_state = KEYBOARD_PARITY;
    }
    break;
  case KEYBOARD_PARITY:
    keyboard_state = KEYBOARD_END;
    break;
  case KEYBOARD_END:
    keyboard_state = KEYBOARD_START;
    keyboard_buffer[keyboard_buffer_pos] = keyboard_value;
    keyboard_value = 0;
    keyboard_buffer_pos += 1;
    break;
  default:
    break;
  }
}
