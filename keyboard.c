#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_code scan_buffer[BUFFER_SIZE];
volatile unsigned int scan_buffer_head = 0;
volatile unsigned int scan_buffer_tail = 0;

volatile struct scan_code code;
volatile struct scan_state state;

volatile struct scan_code *scan_buffer_remove(void)
{
  volatile struct scan_code *code = NULL;
  if (scan_buffer_head != scan_buffer_tail) {
    code = &scan_buffer[scan_buffer_head];
    scan_buffer_head = (scan_buffer_head + 1) % BUFFER_SIZE;
  }
  return code;
}

inline void scan_buffer_insert(volatile struct scan_code code)
{
  scan_buffer[scan_buffer_tail] = code;
  scan_buffer_tail = (scan_buffer_tail + 1) % BUFFER_SIZE;
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
    scan_buffer_insert(code);
  }
  scan_state_transition(&state);
}

void keyboard_init(void)
{
  // put the key code scanner into the initial state
  state.id = SCAN_START;

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;
}
