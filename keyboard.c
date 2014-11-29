#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_code scan_buffer[SCAN_BUFFER_SIZE];
volatile unsigned int scan_buffer_head = 0;
volatile unsigned int scan_buffer_tail = 0;

volatile struct scan_state state = { SCAN_START, 0 };
volatile struct scan_code code;

// avoid volatile qualifier for external consumers of scan codes
struct scan_code code_external;

struct scan_code *scan_buffer_remove(void)
{
  if (scan_buffer_head != scan_buffer_tail) {
    code_external = scan_buffer[scan_buffer_head];
    scan_buffer_head = scan_buffer_increment(scan_buffer_head);
    return &code_external;
  }
  return NULL;
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

int is_code_release(struct scan_code *code)
{
  return code->value == RELEASE_KEY_VALUE;
}

int is_code_extended(struct scan_code *code)
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

void keyboard_state_transition(struct keyboard_state *state, struct scan_code *code)
{
  // reset the state if the previous state represented a complete key event
  // note that modifier key states can extend over multiple key events
  if (state->final) {
    state->final = 0;
    state->release_mode = 0;
    state->extended_mode = 0;
  }

  // handle special scan codes
  if (is_code_extended(code)) {
    state->extended_mode = 1;
    return;
  } else if (is_code_release(code)) {
    state->release_mode = 1;
    return;
  }

  // determine if a modifier key was involved in the key event that produced the given scan code
  uint8_t modifier = 0;
  if (is_code_left_shift(code)) {
    modifier = (1 << MOD_LEFT_SHIFT);
  } else if (is_code_right_shift(code)) {
    modifier = (1 << MOD_RIGHT_SHIFT);
  } else if (is_code_left_ctrl(code)) {
    modifier = (1 << MOD_LEFT_CTRL);
  } else if (is_code_right_ctrl(code)) {
    modifier = (1 << MOD_RIGHT_CTRL);
  }

  if (state->release_mode) {
    state->modifiers &= ~modifier;
  } else {
    state->modifiers |= modifier;
  }

  state->final = 1;
}
