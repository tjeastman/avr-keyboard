#include <stdlib.h>
#include <avr/io.h>

#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"

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
  // determine if a modifier key was involved in the key event that produced the given frame
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

  if (code->release) {
    state->modifiers &= ~modifier;
  } else {
    state->modifiers |= modifier;
  }
}
