#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
  KEYBOARD_START = 0,
  KEYBOARD_SCAN = 1,
  KEYBOARD_PARITY = 2,
  KEYBOARD_END = 3,
} kstate;

void keyboard_init(void);
#endif
