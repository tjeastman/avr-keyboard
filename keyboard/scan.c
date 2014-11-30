#include <stdlib.h>
#include <avr/io.h>

#include "keyboard/protocol.h"
#include "keyboard/scan.h"

int is_code_left_shift(struct scan_code *code)
{
  return code->value == 0x12 && !code->extended;
}

int is_code_right_shift(struct scan_code *code)
{
  return code->value == 0x59 && !code->extended;
}

int is_code_left_ctrl(struct scan_code *code)
{
  return code->value == 0x14 && !code->extended;
}

int is_code_right_ctrl(struct scan_code *code)
{
  return code->value == 0x14 && code->extended;
}

int is_code_left_alt(struct scan_code *code)
{
  return code->value == 0x11 && !code->extended;
}

int is_code_right_alt(struct scan_code *code)
{
  return code->value == 0x11 && code->extended;
}

int is_code_left_gui(struct scan_code *code)
{
  return code->value == 0x1F && code->extended;
}

int is_code_right_gui(struct scan_code *code)
{
  return code->value == 0x27 && code->extended;
}

int scan_state_transition(struct scan_code *code, uint8_t value)
{
  static struct scan_state state = {0, 0, 0};

  if (value == RELEASE_KEY_VALUE) {
    state.release = 1;
  } else if (value == EXTENDED_KEY_VALUE) {
    state.extended = 1;
  } else {
    state.final = 1;
  }

  if (state.final) {
    code->extended = state.extended;
    code->release = state.release;
    code->value = value;
    state.extended = 0;
    state.release = 0;
    state.final = 0;
    return 1;
  } else {
    return 0;
  }
}

/*
 * Read the next scan code from the keyboard.  This function will block waiting for a
 * complete scan code if at least one frame has already been received and is available in
 * the buffer.
 */
int scan_code_read(struct scan_code *code)
{
  if (!frame_buffer_valid()) {
    return 0;
  }

  while (1) {
    frame_value_t value;
    if (frame_buffer_remove(&value)) {
      if (scan_state_transition(code, value)) {
        return 1;
      }
    }
  }
}
