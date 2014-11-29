#include <stdlib.h>
#include <avr/io.h>
#include "protocol.h"
#include "state.h"

int keyboard_shift_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_SHIFT | 0x01 << MOD_RIGHT_SHIFT);
}

int keyboard_ctrl_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_CTRL | 0x01 << MOD_RIGHT_CTRL);
}

void keyboard_state_transition(struct keyboard_state *state, struct frame *frame)
{
  // reset the state if the previous state represented a complete key event
  // note that modifier key states can extend over multiple key events
  if (state->final) {
    state->final = 0;
    state->release_mode = 0;
    state->extended_mode = 0;
  }

  // handle special frames
  if (is_frame_extended(frame)) {
    state->extended_mode = 1;
    return;
  } else if (is_frame_release(frame)) {
    state->release_mode = 1;
    return;
  }

  // determine if a modifier key was involved in the key event that produced the given frame
  uint8_t modifier = 0;
  if (is_frame_left_shift(frame)) {
    modifier = (1 << MOD_LEFT_SHIFT);
  } else if (is_frame_right_shift(frame)) {
    modifier = (1 << MOD_RIGHT_SHIFT);
  } else if (is_frame_left_ctrl(frame)) {
    modifier = (1 << MOD_LEFT_CTRL);
  } else if (is_frame_right_ctrl(frame)) {
    modifier = (1 << MOD_RIGHT_CTRL);
  }

  if (state->release_mode) {
    state->modifiers &= ~modifier;
  } else {
    state->modifiers |= modifier;
  }

  state->final = 1;
}
