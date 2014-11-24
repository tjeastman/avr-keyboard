#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_buffer *buffer_head;
volatile struct scan_buffer *buffer_tail;
volatile struct scan_buffer *buffer_current;

void keyboard_init(void) {
  int i;

  // initialize empty circular linked list buffer of scan codes
  buffer_head = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
  buffer_tail = buffer_head;
  buffer_current = buffer_head;
  for (i = 1; i < BUFFER_SIZE; ++i) {
    buffer_current->next = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
    buffer_current = buffer_current->next;
  }
  buffer_current->next = buffer_head;

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;
}

void keyboard_interrupt(void) {
  static enum scan_state state = SCAN_START;
  static struct scan_code c;

  switch (state) {
  case SCAN_START:
    state = SCAN_DATA;
    c.value = 0;
    c.nbits = 0;
    c.parity = 0;
    break;
  case SCAN_DATA:
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
    // put the scan code into the buffer
    buffer_head->state = SCAN_END;
    buffer_head->code.value = c.value;
    buffer_head->code.nbits = c.nbits;
    buffer_head->code.parity = c.parity;
    buffer_head = buffer_head->next;
    break;
  default:
    break;
  }
}
