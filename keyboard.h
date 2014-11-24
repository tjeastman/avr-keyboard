#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFER_SIZE 10

enum scan_state {
  SCAN_START = 0,
  SCAN_DATA = 1,
  SCAN_PARITY = 2,
  SCAN_END = 3,
};

struct scan_code {
  uint8_t value;
  uint8_t nbits; // number of bits received from the device
  uint8_t parity;
};

struct scan_buffer {
  enum scan_state state;
  struct scan_code code;
  volatile struct scan_buffer *next;
};

void keyboard_init(void);
void keyboard_interrupt(void);

extern volatile struct scan_buffer *buffer_head;
extern volatile struct scan_buffer *buffer_tail;

#endif
