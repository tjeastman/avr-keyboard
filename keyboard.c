#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_buffer *buffer_head;
volatile struct scan_buffer *buffer_tail;
volatile struct scan_buffer *buffer_current;

volatile struct scan_code code;
volatile struct scan_state state;

volatile struct scan_code *buffer_remove(void)
{
  volatile struct scan_code *c = NULL;
  if (buffer_head != buffer_tail) {
    c = &buffer_tail->code;
    buffer_tail = buffer_tail->next;
  }
  return c;
}

inline void buffer_insert(volatile struct scan_code code)
{
  buffer_head->code.value = code.value;
  buffer_head->code.parity = code.parity;
  buffer_head = buffer_head->next;
}

inline void scan_state_transition(volatile struct scan_state *state)
{
  if (state->id == SCAN_DATA)
    state->nbits += 1;

  if (state->id == SCAN_START) {
    state->id = SCAN_DATA;
    state->nbits = 0;
  } else if (state->id == SCAN_DATA && state->nbits < 8) {
    state->id = SCAN_DATA;
  } else if (state->id == SCAN_DATA && state->nbits == 8) {
    state->id = SCAN_PARITY;
  } else if (state->id == SCAN_PARITY) {
    state->id = SCAN_END;
  } else if (state->id == SCAN_END) {
    state->id = SCAN_START;
  }
}

void keyboard_interrupt(void)
{
  if (state.id == SCAN_START) {
    code.value = 0;
    code.parity = 0;
  } else if (state.id == SCAN_DATA) {
    code.value = code.value >> 1;
    if (PIND & (1<<PD3))
      code.value |= 0x80;
  } else if (state.id ==  SCAN_PARITY) {
    code.parity = PIND & (1<<PD3) ? 1 : 0;
  } else if (state.id == SCAN_END) {
    // put the scan code into the buffer
    buffer_insert(code);
  }
  scan_state_transition(&state);
}

void keyboard_init(void)
{
  // initialize empty circular linked list buffer of scan codes
  buffer_head = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
  buffer_tail = buffer_head;
  buffer_current = buffer_head;
  for (int i = 1; i < BUFFER_SIZE; ++i) {
    buffer_current->next = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
    buffer_current = buffer_current->next;
  }
  buffer_current->next = buffer_head;

  // put the key code scanner into the initial state
  state.id = SCAN_START;

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;
}
