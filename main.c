#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "keyboard.h"

setup_keyboard_interrupt();

void keyboard_state_transition(struct keyboard_state *state, struct scan_code *code)
{
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
}

struct key {
  uint8_t value;
  char *label;
  char *label_shift;
};

struct key_page {
  struct key *keys;
  unsigned int size;
};

struct key keys[] =
  {
    {0x01, "[F9]", NULL},
    {0x03, "[F5]", NULL},
    {0x04, "[F3]", NULL},
    {0x05, "[F1]", NULL},
    {0x06, "[F2]", NULL},
    {0x07, "[F12]", NULL},
    {0x09, "[F10]", NULL},
    {0x0A, "[F8]", NULL},
    {0x0B, "[F6]", NULL},
    {0x0C, "[F4]", NULL},
    {0x0D, "\t", NULL},
    {0x0E, "`", "~"},
    {0x15, "q", "Q"},
    {0x16, "1", "!"},
    {0x1A, "z", "Z"},
    {0x1B, "s", "S"},
    {0x1C, "a", "A"},
    {0x1D, "w", "W"},
    {0x1E, "2", "@"},
    {0x21, "c", "C"},
    {0x22, "x", "X"},
    {0x23, "d", "D"},
    {0x24, "e", "E"},
    {0x25, "4", "$"},
    {0x26, "3", "#"},
    {0x29, " ", NULL},
    {0x2A, "v", "V"},
    {0x2B, "f", "F"},
    {0x2C, "t", "T"},
    {0x2D, "r", "R"},
    {0x2E, "5", "%"},
    {0x31, "n", "N"},
    {0x32, "b", "B"},
    {0x33, "h", "H"},
    {0x34, "g", "G"},
    {0x35, "y", "Y"},
    {0x36, "6", "^"},
    {0x3A, "m", "M"},
    {0x3B, "j", "J"},
    {0x3C, "u", "U"},
    {0x3D, "7", "&"},
    {0x3E, "8", "*"},
    {0x41, ",", "<"},
    {0x42, "k", "K"},
    {0x43, "i", "I"},
    {0x44, "o", "O"},
    {0x45, "0", ")"},
    {0x46, "9", "("},
    {0x49, ".", ">"},
    {0x4A, "/", "?"},
    {0x4B, "l", "L"},
    {0x4C, ";", ":"},
    {0x4D, "p", "P"},
    {0x4E, "-", "_"},
    {0x52, "'", "\""},
    {0x54, "[", "{"},
    {0x55, "=", "+"},
    {0x58, "[CAPS]", NULL},
    {0x5A, "\r\n", NULL},
    {0x5B, "]", "}"},
    {0x5D, "\\", "|"},
    {0x66, "[BACKSPACE]", NULL},
    {0x76, "[ESC]", NULL},
    {0x77, "[NUM]", NULL},
    {0x78, "[F11]", NULL},
    {0x7E, "[SCROLL]", NULL},
    {0x83, "[F7]", NULL}
  };

struct key extended_keys[] =
  {
    {0x69, "[END]", NULL},
    {0x6B, "[LEFT]", NULL},
    {0x6C, "[HOME]", NULL},
    {0x70, "[INS]", NULL},
    {0x71, "[DEL]", NULL},
    {0x72, "[DOWN]", NULL},
    {0x74, "[RIGHT]", NULL},
    {0x75, "[UP]", NULL},
    {0x7A, "[PGDOWN]", NULL},
    {0x7D, "[PGUP]", NULL}
  };

struct key_page default_key_page = { keys, 67 };
struct key_page extended_key_page = { extended_keys, 10 };

int compare_keys(const void *k1, const void *k2)
{
  struct key *key1 = (struct key *)k1;
  struct key *key2 = (struct key *)k2;
  return key1->value - key2->value;
}

struct key *lookup_key(struct scan_code *code, struct key_page *current_keys)
{
  struct key search_key;
  search_key.value = code->value;
  return bsearch(&search_key, current_keys->keys, current_keys->size, sizeof(struct key), compare_keys);
}

char *decode(struct scan_code *code)
{
  static struct keyboard_state state = { 0, 0 };
  static struct key_page *current_keys = &default_key_page;
  struct key *found_key;
  char *label = NULL;

  keyboard_state_transition(&state, code);

  if (is_code_extended(code)) {
    // swap in a new "page" of scan codes
    current_keys = &extended_key_page;
    return NULL;
  }

  if (state.release_mode) {
    state.release_mode = 0;
  } else if (is_code_release(code)) {
    state.release_mode = 1;
  } else if (keyboard_shift_pressed(&state)) {
    found_key = lookup_key(code, current_keys);
    if (found_key) {
      if (found_key->label_shift) {
        label = found_key->label_shift;
      } else {
        label = found_key->label;
      }
    }
  } else {
    found_key = lookup_key(code, current_keys);
    if (found_key) {
      label = found_key->label;
    }
  }

  // switch back to the default "page" of scan codes
  current_keys = &default_key_page;

  return label;
}

int main(void)
{
  struct scan_code *code;
  char *label;
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  printf("Hello [%d]\r\n", sizeof(keys));

  while (1) {
    if (code = scan_buffer_remove()) {
      if (label = decode(code)) {
        printf("%s", label);
      }
    }
    _delay_ms(10);
  }

  return 0;
}
