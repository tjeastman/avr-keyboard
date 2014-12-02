#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>

#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"

struct key_map keys[] = {
  {{0x01, 0, 0}, 0x42},
  {{0x03, 0, 0}, 0x3E},
  {{0x04, 0, 0}, 0x3C},
  {{0x05, 0, 0}, 0x3A},
  {{0x06, 0, 0}, 0x3B},
  {{0x07, 0, 0}, 0x45},
  {{0x09, 0, 0}, 0x43},
  {{0x0A, 0, 0}, 0x41},
  {{0x0B, 0, 0}, 0x3F},
  {{0x0C, 0, 0}, 0x3D},
  {{0x0D, 0, 0}, 0x2B},
  {{0x0E, 0, 0}, 0x35},
  {{0x11, 0, 0}, 0xE2},
  {{0x11, 1, 0}, 0xE6},
  {{0x12, 0, 0}, 0xE1},
  {{0x14, 0, 0}, 0xE0},
  {{0x14, 1, 0}, 0xE4},
  {{0x15, 0, 0}, 0x14},
  {{0x16, 0, 0}, 0x1E},
  {{0x1A, 0, 0}, 0x1D},
  {{0x1B, 0, 0}, 0x16},
  {{0x1C, 0, 0}, 0x04},
  {{0x1D, 0, 0}, 0x1A},
  {{0x1E, 0, 0}, 0x1F},
  {{0x21, 0, 0}, 0x06},
  {{0x22, 0, 0}, 0x1B},
  {{0x23, 0, 0}, 0x07},
  {{0x24, 0, 0}, 0x08},
  {{0x25, 0, 0}, 0x21},
  {{0x26, 0, 0}, 0x20},
  {{0x29, 0, 0}, 0x2C},
  {{0x2A, 0, 0}, 0x19},
  {{0x2B, 0, 0}, 0x09},
  {{0x2C, 0, 0}, 0x17},
  {{0x2D, 0, 0}, 0x15},
  {{0x2E, 0, 0}, 0x22},
  {{0x31, 0, 0}, 0x11},
  {{0x32, 0, 0}, 0x05},
  {{0x33, 0, 0}, 0x0B},
  {{0x34, 0, 0}, 0x0A},
  {{0x35, 0, 0}, 0x1C},
  {{0x36, 0, 0}, 0x23},
  {{0x3A, 0, 0}, 0x10},
  {{0x3B, 0, 0}, 0x0D},
  {{0x3C, 0, 0}, 0x18},
  {{0x3D, 0, 0}, 0x24},
  {{0x3E, 0, 0}, 0x25},
  {{0x41, 0, 0}, 0x36},
  {{0x42, 0, 0}, 0x0E},
  {{0x43, 0, 0}, 0x0C},
  {{0x44, 0, 0}, 0x12},
  {{0x45, 0, 0}, 0x27},
  {{0x46, 0, 0}, 0x26},
  {{0x49, 0, 0}, 0x37},
  {{0x4A, 0, 0}, 0x38},
  {{0x4B, 0, 0}, 0x0F},
  {{0x4C, 0, 0}, 0x33},
  {{0x4D, 0, 0}, 0x13},
  {{0x4E, 0, 0}, 0x2D},
  {{0x52, 0, 0}, 0x34},
  {{0x54, 0, 0}, 0x2F},
  {{0x55, 0, 0}, 0x2E},
  {{0x58, 0, 0}, 0x39},
  {{0x59, 0, 0}, 0xE5},
  {{0x5A, 0, 0}, 0x28},
  {{0x5B, 0, 0}, 0x30},
  {{0x5D, 0, 0}, 0x31},
  {{0x66, 0, 0}, 0x2A},
  {{0x69, 1, 0}, 0x4D},
  {{0x6B, 1, 0}, 0x50},
  {{0x6C, 1, 0}, 0x4A},
  {{0x70, 1, 0}, 0x49},
  {{0x71, 1, 0}, 0x4C},
  {{0x72, 1, 0}, 0x51},
  {{0x74, 1, 0}, 0x4F},
  {{0x75, 1, 0}, 0x52},
  {{0x76, 0, 0}, 0x29},
  {{0x77, 0, 0}, 0x53},
  {{0x78, 0, 0}, 0x44},
  {{0x7A, 1, 0}, 0x4E},
  {{0x7D, 1, 0}, 0x4B},
  {{0x7E, 0, 0}, 0x47},
  {{0x83, 0, 0}, 0x40}
};

int key_compare(const void *k1, const void *k2)
{
  struct key_map *key1 = (struct key_map *)k1;
  struct key_map *key2 = (struct key_map *)k2;
  if (key1->code.value == key2->code.value) {
    return key1->code.extended - key2->code.extended;
  } else {
    return key1->code.value - key2->code.value;
  }
}

bool key_search(struct scan_code *code, struct key_event *event)
{
  struct key_map search;
  struct key_map *result;

  search.code.value = code->value;
  search.code.extended = code->extended;

  int size = sizeof(keys) / sizeof(struct key_map);

  result = bsearch(&search, keys, size, sizeof(struct key_map), key_compare);

  if (result) {
    event->value = result->value;
    event->release = code->release;
    return true;
  } else {
    return false;
  }
}

bool is_key_left_shift(uint8_t value)
{
  return value == 0xE1;
}

bool is_key_right_shift(uint8_t value)
{
  return value == 0xE5;
}

bool is_key_left_ctrl(uint8_t value)
{
  return value == 0xE0;
}

bool is_key_right_ctrl(uint8_t value)
{
  return value == 0xE4;
}

bool is_key_left_alt(uint8_t value)
{
  return value == 0xE2;
}

bool is_key_right_alt(uint8_t value)
{
  return value == 0xE6;
}

bool is_key_left_gui(uint8_t value)
{
  return value == 0xE3;
}

bool is_key_right_gui(uint8_t value)
{
  return value == 0xE7;
}

bool keyboard_shift_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_SHIFT | 0x01 << MOD_RIGHT_SHIFT);
}

bool keyboard_ctrl_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_CTRL | 0x01 << MOD_RIGHT_CTRL);
}

bool keyboard_alt_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_ALT | 0x01 << MOD_RIGHT_ALT);
}

bool keyboard_gui_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_GUI | 0x01 << MOD_RIGHT_GUI);
}

/*
 * Update keyboard state to reflect a given key event.
 */
void keyboard_state_update(struct keyboard_state *state, struct key_event *event)
{
  // determine if the given scan code involves a modifier key
  uint8_t modifier = 0;
  if (is_key_left_shift(event->value)) {
    modifier = (1 << MOD_LEFT_SHIFT);
  } else if (is_key_right_shift(event->value)) {
    modifier = (1 << MOD_RIGHT_SHIFT);
  } else if (is_key_left_ctrl(event->value)) {
    modifier = (1 << MOD_LEFT_CTRL);
  } else if (is_key_right_ctrl(event->value)) {
    modifier = (1 << MOD_RIGHT_CTRL);
  } else if (is_key_left_alt(event->value)) {
    modifier = (1 << MOD_LEFT_ALT);
  } else if (is_key_right_alt(event->value)) {
    modifier = (1 << MOD_RIGHT_ALT);
  } else if (is_key_left_gui(event->value)) {
    modifier = (1 << MOD_LEFT_GUI);
  } else if (is_key_right_gui(event->value)) {
    modifier = (1 << MOD_RIGHT_GUI);
  }

  if (modifier) {
    if (event->release) {
      state->modifiers &= ~modifier;
    } else {
      state->modifiers |= modifier;
    }
  }
}
