#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>

#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"
#include "keyboard/label.h"

struct key_label_map key_labels[] = {
  {0x42, "[F9]", NULL},
  {0x3E, "[F5]", NULL},
  {0x3C, "[F3]", NULL},
  {0x3A, "[F1]", NULL},
  {0x3B, "[F2]", NULL},
  {0x45, "[F12]", NULL},
  {0x43, "[F10]", NULL},
  {0x41, "[F8]", NULL},
  {0x3F, "[F6]", NULL},
  {0x3D, "[F4]", NULL},
  {0x2B, "\t", NULL},
  {0x35, "`", "~"},
  {0xE2, NULL, NULL}, // left alt
  {0xE6, NULL, NULL}, // right alt
  {0xE1, NULL, NULL}, // left shift
  {0xE0, NULL, NULL}, // left control
  {0xE4, NULL, NULL}, // right control
  {0x14, "q", "Q"},
  {0x1E, "1", "!"},
  {0x1D, "z", "Z"},
  {0x16, "s", "S"},
  {0x04, "a", "A"},
  {0x1A, "w", "W"},
  {0x1F, "2", "@"},
  {0x06, "c", "C"},
  {0x1B, "x", "X"},
  {0x07, "d", "D"},
  {0x08, "e", "E"},
  {0x21, "4", "$"},
  {0x20, "3", "#"},
  {0x2C, " ", NULL},
  {0x19, "v", "V"},
  {0x09, "f", "F"},
  {0x17, "t", "T"},
  {0x15, "r", "R"},
  {0x22, "5", "%"},
  {0x11, "n", "N"},
  {0x05, "b", "B"},
  {0x0B, "h", "H"},
  {0x0A, "g", "G"},
  {0x1C, "y", "Y"},
  {0x23, "6", "^"},
  {0x10, "m", "M"},
  {0x0D, "j", "J"},
  {0x18, "u", "U"},
  {0x24, "7", "&"},
  {0x25, "8", "*"},
  {0x36, ",", "<"},
  {0x0E, "k", "K"},
  {0x0C, "i", "I"},
  {0x12, "o", "O"},
  {0x27, "0", ")"},
  {0x26, "9", "("},
  {0x37, ".", ">"},
  {0x38, "/", "?"},
  {0x0F, "l", "L"},
  {0x33, ";", ":"},
  {0x13, "p", "P"},
  {0x2D, "-", "_"},
  {0x34, "'", "\""},
  {0x2F, "[", "{{"},
  {0x2E, "=", "+"},
  {0x39, "[CAPS]", NULL},
  {0xE5, NULL, NULL}, // right shift
  {0x28, "\r\n", NULL},
  {0x30, "]", "}"},
  {0x31, "\\", "|"},
  {0x2A, "[BACKSPACE]", NULL},
  {0x4D, "[END]", NULL},
  {0x50, "[LEFT]", NULL},
  {0x4A, "[HOME]", NULL},
  {0x49, "[INS]", NULL},
  {0x4C, "[DEL]", NULL},
  {0x51, "[DOWN]", NULL},
  {0x4F, "[RIGHT]", NULL},
  {0x52, "[UP]", NULL},
  {0x29, "[ESC]", NULL},
  {0x53, "[NUM]", NULL},
  {0x44, "[F11]", NULL},
  {0x4E, "[PGDOWN]", NULL},
  {0x4B, "[PGUP]", NULL},
  {0x47, "[SCROLL]", NULL},
  {0x40, "[F7]", NULL}
};

int key_label_compare(const void *k1, const void *k2)
{
  struct key_label_map *key1 = (struct key_label_map *)k1;
  struct key_label_map *key2 = (struct key_label_map *)k2;
  return key1->value - key2->value;
}

/*
 * Get an appropriate string label for the given key event and keyboard state if one is
 * available.  If no label is available then return NULL.
 */
char *key_label(struct keyboard_state *state, struct key_event *event)
{
  if (event->release) {
    return NULL;
  }

  struct key_label_map search;
  struct key_label_map *result;

  search.value = event->value;
  search.label_base = NULL;
  search.label_shift = NULL;

  size_t key_label_size = sizeof(struct key_label_map);
  size_t num_key_labels = sizeof(key_labels) / key_label_size;

  result = bsearch(&search, key_labels, num_key_labels, key_label_size, key_label_compare);

  if (!result) {
    return NULL;
  } else if (keyboard_shift_pressed(state) && result->label_shift) {
    return result->label_shift;
  } else {
    return result->label_base;
  }
}
