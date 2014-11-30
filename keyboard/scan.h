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
  uint8_t extended;
  uint8_t release;
  uint8_t value;
};

int is_code_left_shift(struct scan_code *);
int is_code_right_shift(struct scan_code *);
int is_code_left_ctrl(struct scan_code *);
int is_code_right_ctrl(struct scan_code *);
int is_code_left_alt(struct scan_code *code);
int is_code_right_alt(struct scan_code *code);
int is_code_left_gui(struct scan_code *code);
int is_code_right_gui(struct scan_code *code);

int scan_state_transition(struct scan_code *, uint8_t);

#endif
