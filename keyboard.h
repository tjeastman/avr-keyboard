#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
  KEYBOARD_START = 0,
  KEYBOARD_SCAN = 1,
  KEYBOARD_PARITY = 2,
  KEYBOARD_END = 3,
} scan_state;

typedef struct {
  uint8_t value;
  uint8_t nbits; // number of bits received from the device
  uint8_t parity;
} scan_code;

void keyboard_init(void);
int get_buffer_size(void);
unsigned char get_buffer_item(int);
void reset_buffer(void);
void keyboard_interrupt(void);

extern volatile unsigned char *keyboard_buffer;
extern volatile int keyboard_buffer_pos;

#endif
