#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "keyboard.h"

unsigned char keyboard_buffer[100];
int keyboard_buffer_pos = 0;

int get_buffer_size(void) {
  return keyboard_buffer_pos;
}

unsigned char get_buffer_item(int i) {
  return keyboard_buffer[i];
}

void reset_buffer(void) {
  keyboard_buffer_pos = 0;
}

ISR(INT0_vect) {
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
    break; // should not get here
  }
}

int main(void) {
  int i, size, item;
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  // enable interrupts
  sei();

  while (1) {
    size = get_buffer_size();
    for (i = 0; i < size; ++i) {
      item = get_buffer_item(i);
      printf("0x%x [%d] ", item, item);
    }
    if (size > 0) {
      printf("\r\n");
    }
    reset_buffer();
    _delay_ms(4000);
  }

  return 0;
}
