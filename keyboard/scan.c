#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>

#include "keyboard/protocol.h"
#include "keyboard/scan.h"

bool scan_state_transition(struct scan_code *code, frame_value_t value)
{
  static struct scan_state state = {false, false, false};

  if (value == RELEASE_KEY_VALUE) {
    state.release = true;
  } else if (value == EXTENDED_KEY_VALUE) {
    state.extended = true;
  } else {
    state.final = true;
  }

  if (state.final) {
    code->extended = state.extended;
    code->release = state.release;
    code->value = value;
    state.extended = false;
    state.release = false;
    state.final = false;
    return true;
  }
  return false;
}

bool scan_code_read(struct scan_code *code)
{
  frame_value_t value;
  if (!frame_buffer_valid()) {
    return false;
  } else if (!frame_buffer_remove(&value)){
    return false;
  } else if (!scan_state_transition(code, value)){
    return false;
  }
  return true;
}
