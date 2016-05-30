#ifndef AVR_KEYBOARD_SCAN_H_
#define AVR_KEYBOARD_SCAN_H_

#define RELEASE_KEY_VALUE 0xF0
#define EXTENDED_KEY_VALUE 0xE0

struct scan_state {
  bool extended;
  bool release;
  bool final;
};

struct scan_code {
  frame_value_t value;
  bool extended;
  bool release;
};

bool scan_code_read(struct scan_code *);

#endif
