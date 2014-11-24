#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFER_SIZE 10

enum scan_state
{
  SCAN_START,
  SCAN_DATA,
  SCAN_PARITY,
  SCAN_END
};

struct scan_code
{
  uint8_t value;
  uint8_t nbits; // number of bits received from the device
  uint8_t parity;
  enum scan_state state;
};

struct scan_buffer
{
  struct scan_code code;
  volatile struct scan_buffer *next;
};

void keyboard_init(void);
void keyboard_interrupt(void);

extern volatile struct scan_buffer *buffer_head;
extern volatile struct scan_buffer *buffer_tail;

#endif
