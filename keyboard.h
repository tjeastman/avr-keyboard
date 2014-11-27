#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFER_SIZE 10

enum scan_state_id
{
  SCAN_START,
  SCAN_DATA,
  SCAN_PARITY,
  SCAN_END
};

struct scan_state
{
  uint8_t nbits; // number of bits received from the device
  enum scan_state_id id;
};

struct scan_code
{
  uint8_t value;
  uint8_t parity;
};

void keyboard_init(void);
void keyboard_interrupt(void);

volatile struct scan_code *buffer_remove(void);

#endif
