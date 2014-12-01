#ifndef AVR_KEYBOARD_SCAN_H_
#define AVR_KEYBOARD_SCAN_H_

#define RELEASE_KEY_VALUE 0xF0
#define EXTENDED_KEY_VALUE 0xE0

struct scan_state {
  uint8_t extended;
  uint8_t release;
  uint8_t final;
};

struct scan_code {
  uint8_t value;
  uint8_t extended;
  uint8_t release;
};

int scan_code_read(struct scan_code *);

#endif
