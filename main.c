#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "keyboard.h"

setup_keyboard_interrupt();

void keyboard_state_transition(struct keyboard_state *state, struct scan_code *code)
{
  if (state->release_mode) {
    if (is_code_left_shift(code)) {
      state->modifiers &= ~(1 << MOD_LEFT_SHIFT);
    } else if (is_code_right_shift(code)) {
      state->modifiers &= ~(1 << MOD_RIGHT_SHIFT);
    } else if (is_code_left_ctrl(code)) {
      state->modifiers &= ~(1 << MOD_LEFT_CTRL);
    } else if (is_code_right_ctrl(code)) {
      state->modifiers &= ~(1 << MOD_RIGHT_CTRL);
    }
  } else {
    if (is_code_left_shift(code)) {
      state->modifiers |= (1 << MOD_LEFT_SHIFT);
    } else if (is_code_right_shift(code)) {
      state->modifiers |= (1 << MOD_RIGHT_SHIFT);
    } else if (is_code_left_ctrl(code)) {
      state->modifiers |= (1 << MOD_LEFT_CTRL);
    } else if (is_code_right_ctrl(code)) {
      state->modifiers |= (1 << MOD_RIGHT_CTRL);
    }
  }
}

struct key {
  char *label;
  uint8_t value;
};

struct key_page {
  struct key *keys;
  struct key *shift_keys;
  unsigned int size;
  unsigned int shift_size;
};

struct key keys[] =
  {
    {"[F9]", 0x01},
    {"[F5]", 0x03},
    {"[F3]", 0x04},
    {"[F1]", 0x05},
    {"[F2]", 0x06},
    {"[F12]", 0x07},
    {"[F10]", 0x09},
    {"[F8]", 0x0A},
    {"[F6]", 0x0B},
    {"[F4]", 0x0C},
    {"\t", 0x0D},
    {"`", 0x0E},
    {"q", 0x15},
    {"1", 0x16},
    {"z", 0x1A},
    {"s", 0x1B},
    {"a", 0x1C},
    {"w", 0x1D},
    {"2", 0x1E},
    {"c", 0x21},
    {"x", 0x22},
    {"d", 0x23},
    {"e", 0x24},
    {"4", 0x25},
    {"3", 0x26},
    {" ", 0x29},
    {"v", 0x2A},
    {"f", 0x2B},
    {"t", 0x2C},
    {"r", 0x2D},
    {"5", 0x2E},
    {"n", 0x31},
    {"b", 0x32},
    {"h", 0x33},
    {"g", 0x34},
    {"y", 0x35},
    {"6", 0x36},
    {"m", 0x3A},
    {"j", 0x3B},
    {"u", 0x3C},
    {"7", 0x3D},
    {"8", 0x3E},
    {",", 0x41},
    {"k", 0x42},
    {"i", 0x43},
    {"o", 0x44},
    {"0", 0x45},
    {"9", 0x46},
    {".", 0x49},
    {"/", 0x4A},
    {"l", 0x4B},
    {";", 0x4C},
    {"p", 0x4D},
    {"-", 0x4E},
    {"'", 0x52},
    {"[", 0x54},
    {"=", 0x55},
    {"[CAPS]", 0x58},
    {"\r\n", 0x5A},
    {"]", 0x5B},
    {"\\", 0x5D},
    {"[BACKSPACE]", 0x66},
    {"[ESC]", 0x76},
    {"[NUM]", 0x77},
    {"[F11]", 0x78},
    {"[SCROLL]", 0x7E},
    {"[F7]", 0x83}
  };

struct key shift_keys[] =
  {
    {"~", 0x0E},
    {"Q", 0x15},
    {"!", 0x16},
    {"Z", 0x1A},
    {"S", 0x1B},
    {"A", 0x1C},
    {"W", 0x1D},
    {"@", 0x1E},
    {"C", 0x21},
    {"X", 0x22},
    {"D", 0x23},
    {"E", 0x24},
    {"$", 0x25},
    {"#", 0x26},
    {"V", 0x2A},
    {"F", 0x2B},
    {"T", 0x2C},
    {"R", 0x2D},
    {"%", 0x2E},
    {"N", 0x31},
    {"B", 0x32},
    {"H", 0x33},
    {"G", 0x34},
    {"Y", 0x35},
    {"^", 0x36},
    {"M", 0x3A},
    {"J", 0x3B},
    {"U", 0x3C},
    {"&", 0x3D},
    {"*", 0x3E},
    {"<", 0x41},
    {"K", 0x42},
    {"I", 0x43},
    {"O", 0x44},
    {")", 0x45},
    {"(", 0x46},
    {">", 0x49},
    {"?", 0x4A},
    {"L", 0x4B},
    {":", 0x4C},
    {"P", 0x4D},
    {"_", 0x4E},
    {"\"", 0x52},
    {"{", 0x54},
    {"+", 0x55},
    {"}", 0x5B},
    {"|", 0x5D},
  };

struct key extended_keys[] =
  {
    {"[END]", 0x69},
    {"[LEFT]", 0x6B},
    {"[HOME]", 0x6C},
    {"[INS]", 0x70},
    {"[DEL]", 0x71},
    {"[DOWN]", 0x72},
    {"[RIGHT]", 0x74},
    {"[UP]", 0x75},
    {"[PGDOWN]", 0x7A},
    {"[PGUP]", 0x7D}
  };

struct key_page default_key_page = { keys, shift_keys, 67, 47 };
struct key_page extended_key_page = { extended_keys, NULL, 10, 0 };

int compare_keys(const void *k1, const void *k2)
{
  struct key *key1 = (struct key *)k1;
  struct key *key2 = (struct key *)k2;
  return key1->value - key2->value;
}

char *lookup_key(struct scan_code *code, struct key_page *current_keys)
{
  struct key search_key;
  struct key *found_key;
  search_key.value = code->value;
  found_key = bsearch(&search_key, current_keys->keys, current_keys->size, sizeof(struct key), compare_keys);
  if (found_key) {
    return found_key->label;
  } else {
    return NULL;
  }
}

char *lookup_shift_key(struct scan_code *code, struct key_page *current_keys)
{
  struct key search_key;
  struct key *found_key;
  search_key.value = code->value;
  found_key = bsearch(&search_key, current_keys->shift_keys, current_keys->shift_size, sizeof(struct key), compare_keys);
  if (found_key) {
    return found_key->label;
  } else {
    return NULL;
  }
}

char *decode(struct scan_code *code)
{
  static struct keyboard_state state = { 0, 0 };
  static struct key_page *current_keys = &default_key_page;
  char *label = NULL;
  struct scan_code new_code = *code;

  keyboard_state_transition(&state, &new_code);

  if (is_extended_code(code)) {
    // swap in a new "page" of scan codes
    current_keys = &extended_key_page;
    return NULL;
  }

  if (state.release_mode) {
    state.release_mode = 0;
  } else if (is_release_code(code)) {
    state.release_mode = 1;
  } else if (keyboard_shift_pressed(&state)) {
    label = lookup_shift_key(code, current_keys);
    if (!label) {
      label = lookup_key(code, current_keys);
    }
  } else {
    label = lookup_key(code, current_keys);
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
