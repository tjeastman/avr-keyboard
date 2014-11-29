#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_code scan_buffer[SCAN_BUFFER_SIZE];
volatile unsigned int scan_buffer_head = 0;
volatile unsigned int scan_buffer_tail = 0;

volatile struct scan_state state = { SCAN_START, 0 };
volatile struct scan_code code;

volatile struct scan_code *scan_buffer_remove(void)
{
  volatile struct scan_code *code = NULL;
  if (scan_buffer_head != scan_buffer_tail) {
    code = &scan_buffer[scan_buffer_head];
    scan_buffer_head = scan_buffer_increment(scan_buffer_head);
  }
  return code;
}

inline void scan_buffer_insert(volatile struct scan_code code)
{
  scan_buffer[scan_buffer_tail] = code;
  scan_buffer_tail = scan_buffer_increment(scan_buffer_tail);
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
    code.value |= (read_keyboard_data() << 7);
  } else if (state.id ==  SCAN_PARITY) {
    code.parity = read_keyboard_data();
  } else if (state.id == SCAN_END) {
    scan_buffer_insert(code);
  }
  scan_state_transition(&state);
}

void keyboard_init(void)
{
  // enable pull-up resistors
  KEYBOARD_PORT |= _BV(KEYBOARD_CLOCK_PIN_NUM) | _BV(KEYBOARD_DATA_PIN_NUM);

  // enable the keyboard interrupt
  EIMSK |= _BV(KEYBOARD_INTERRUPT);
  // trigger the keyboard interrupt on the falling edge
  EICRA |= _BV(KEYBOARD_INTERRUPT_CTRL);

  // enable interrupts
  sei();
}

int is_release_code(volatile struct scan_code *code)
{
  return code->value == RELEASE_KEY_VALUE;
}

int is_extended_code(volatile struct scan_code *code)
{
  return code->value == EXTENDED_KEY_VALUE;
}

int is_code_left_shift(struct scan_code *code)
{
  return code->value == 0x12;
}

int is_code_right_shift(struct scan_code *code)
{
  return code->value == 0x59;
}

int is_code_left_ctrl(struct scan_code *code)
{
  return code->value == 0;
}

int is_code_right_ctrl(struct scan_code *code)
{
  return code->value == 0;
}

int keyboard_shift_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_SHIFT | 0x01 << MOD_RIGHT_SHIFT);
}

int keyboard_ctrl_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_CTRL | 0x01 << MOD_RIGHT_CTRL);
}
