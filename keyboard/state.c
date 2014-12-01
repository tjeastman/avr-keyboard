#include <stdlib.h>
#include <avr/io.h>

#include "keyboard/scan.h"
#include "keyboard/state.h"

struct key keys[] = {
  {0x01, 0, 0x42, "[F9]", NULL},
  {0x03, 0, 0x3E, "[F5]", NULL},
  {0x04, 0, 0x3C, "[F3]", NULL},
  {0x05, 0, 0x3A, "[F1]", NULL},
  {0x06, 0, 0x3B, "[F2]", NULL},
  {0x07, 0, 0x45, "[F12]", NULL},
  {0x09, 0, 0x43, "[F10]", NULL},
  {0x0A, 0, 0x41, "[F8]", NULL},
  {0x0B, 0, 0x3F, "[F6]", NULL},
  {0x0C, 0, 0x3D, "[F4]", NULL},
  {0x0D, 0, 0x2B, "\t", NULL},
  {0x0E, 0, 0x35, "`", "~"},
  {0x11, 0, 0xE2, NULL, NULL}, // left alt
  {0x11, 1, 0xE6, NULL, NULL}, // right alt
  {0x12, 0, 0xE1, NULL, NULL}, // left shift
  {0x14, 0, 0xE0, NULL, NULL}, // left control
  {0x14, 1, 0xE4, NULL, NULL}, // right control
  {0x15, 0, 0x14, "q", "Q"},
  {0x16, 0, 0x1E, "1", "!"},
  {0x1A, 0, 0x1D, "z", "Z"},
  {0x1B, 0, 0x16, "s", "S"},
  {0x1C, 0, 0x04, "a", "A"},
  {0x1D, 0, 0x1A, "w", "W"},
  {0x1E, 0, 0x1F, "2", "@"},
  {0x21, 0, 0x06, "c", "C"},
  {0x22, 0, 0x1B, "x", "X"},
  {0x23, 0, 0x07, "d", "D"},
  {0x24, 0, 0x08, "e", "E"},
  {0x25, 0, 0x21, "4", "$"},
  {0x26, 0, 0x20, "3", "#"},
  {0x29, 0, 0x2C, " ", NULL},
  {0x2A, 0, 0x19, "v", "V"},
  {0x2B, 0, 0x09, "f", "F"},
  {0x2C, 0, 0x17, "t", "T"},
  {0x2D, 0, 0x15, "r", "R"},
  {0x2E, 0, 0x22, "5", "%"},
  {0x31, 0, 0x11, "n", "N"},
  {0x32, 0, 0x05, "b", "B"},
  {0x33, 0, 0x0B, "h", "H"},
  {0x34, 0, 0x0A, "g", "G"},
  {0x35, 0, 0x1C, "y", "Y"},
  {0x36, 0, 0x23, "6", "^"},
  {0x3A, 0, 0x10, "m", "M"},
  {0x3B, 0, 0x0D, "j", "J"},
  {0x3C, 0, 0x18, "u", "U"},
  {0x3D, 0, 0x24, "7", "&"},
  {0x3E, 0, 0x25, "8", "*"},
  {0x41, 0, 0x36, ",", "<"},
  {0x42, 0, 0x0E, "k", "K"},
  {0x43, 0, 0x0C, "i", "I"},
  {0x44, 0, 0x12, "o", "O"},
  {0x45, 0, 0x27, "0", ")"},
  {0x46, 0, 0x26, "9", "("},
  {0x49, 0, 0x37, ".", ">"},
  {0x4A, 0, 0x38, "/", "?"},
  {0x4B, 0, 0x0F, "l", "L"},
  {0x4C, 0, 0x33, ";", ":"},
  {0x4D, 0, 0x13, "p", "P"},
  {0x4E, 0, 0x2D, "-", "_"},
  {0x52, 0, 0x34, "'", "\""},
  {0x54, 0, 0x2F, "[", "{"},
  {0x55, 0, 0x2E, "=", "+"},
  {0x58, 0, 0x39, "[CAPS]", NULL},
  {0x59, 0, 0xE5, NULL, NULL}, // right shift
  {0x5A, 0, 0x28, "\r\n", NULL},
  {0x5B, 0, 0x30, "]", "}"},
  {0x5D, 0, 0x31, "\\", "|"},
  {0x66, 0, 0x2A, "[BACKSPACE]", NULL},
  {0x69, 1, 0x00, "[END]", NULL},
  {0x6B, 1, 0x00, "[LEFT]", NULL},
  {0x6C, 1, 0x00, "[HOME]", NULL},
  {0x70, 1, 0x00, "[INS]", NULL},
  {0x71, 1, 0x00, "[DEL]", NULL},
  {0x72, 1, 0x00, "[DOWN]", NULL},
  {0x74, 1, 0x00, "[RIGHT]", NULL},
  {0x75, 1, 0x00, "[UP]", NULL},
  {0x76, 0, 0x29, "[ESC]", NULL},
  {0x77, 0, 0x53, "[NUM]", NULL},
  {0x78, 0, 0x44, "[F11]", NULL},
  {0x7A, 1, 0x00, "[PGDOWN]", NULL},
  {0x7D, 1, 0x00, "[PGUP]", NULL},
  {0x7E, 0, 0x47, "[SCROLL]", NULL},
  {0x83, 0, 0x40, "[F7]", NULL}
};

int key_compare(const void *k1, const void *k2)
{
  struct key *key1 = (struct key *)k1;
  struct key *key2 = (struct key *)k2;
  if (key1->value == key2->value) {
    return key1->extended - key2->extended;
  } else {
    return key1->value - key2->value;
  }
}

struct key *key_search(struct scan_code *code)
{
  struct key search;
  search.value = code->value;
  search.extended = code->extended;
  int size = sizeof(keys) / sizeof(struct key);
  return bsearch(&search, keys, size, sizeof(struct key), key_compare);
}

char *keyboard_state_label(struct keyboard_state state, struct key *key)
{
  if (keyboard_shift_pressed(&state) && key->label_shift) {
    return key->label_shift;
  } else {
    return key->label;
  }
}

int keyboard_shift_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_SHIFT | 0x01 << MOD_RIGHT_SHIFT);
}

int keyboard_ctrl_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_CTRL | 0x01 << MOD_RIGHT_CTRL);
}

int keyboard_alt_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_ALT | 0x01 << MOD_RIGHT_ALT);
}

int keyboard_gui_pressed(struct keyboard_state *state)
{
  return state->modifiers & (0x01 << MOD_LEFT_GUI | 0x01 << MOD_RIGHT_GUI);
}

void keyboard_state_update(struct keyboard_state *state, struct scan_code *code)
{
  // determine if the given scan code involves a modifier key
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

  if (modifier) {
    if (code->release) {
      state->modifiers &= ~modifier;
    } else {
      state->modifiers |= modifier;
    }
  }
}
