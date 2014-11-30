#include <stdlib.h>
#include <avr/io.h>

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

struct scan_code current_scan_code;
struct scan_code *scan_state_transition(uint8_t code)
{
  static struct scan_state state = {0, 0, 0};

  if (code == RELEASE_KEY_VALUE) {
    state.release = 1;
  } else if (code == EXTENDED_KEY_VALUE) {
    state.extended = 1;
  } else {
    state.final = 1;
  }

  if (state.final) {
    current_scan_code.extended = state.extended;
    current_scan_code.release = state.release;
    current_scan_code.value = code;
    state.extended = 0;
    state.release = 0;
    state.final = 0;
    return &current_scan_code;
  } else {
    return NULL;
  }
}
