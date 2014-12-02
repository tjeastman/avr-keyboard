#ifndef AVR_KEYBOARD_STATE_H_
#define AVR_KEYBOARD_STATE_H_

struct key_map {
  struct scan_code code;
  uint8_t value;
};

struct key_event {
  uint8_t value;
  uint8_t release;
};

enum keyboard_modifier_id
{
  MOD_LEFT_CTRL = 0,
  MOD_LEFT_SHIFT = 1,
  MOD_LEFT_ALT = 2,
  MOD_LEFT_GUI = 3,
  MOD_RIGHT_CTRL = 4,
  MOD_RIGHT_SHIFT = 5,
  MOD_RIGHT_ALT = 6,
  MOD_RIGHT_GUI = 7
};

struct keyboard_state
{
  uint8_t modifiers;
  uint8_t values[1];
};

bool is_key_left_shift(uint8_t);
bool is_key_right_shift(uint8_t);
bool is_key_left_ctrl(uint8_t);
bool is_key_right_ctrl(uint8_t);
bool is_key_left_alt(uint8_t);
bool is_key_right_alt(uint8_t);
bool is_key_left_gui(uint8_t);
bool is_key_right_gui(uint8_t);

bool key_search(struct scan_code *, struct key_event *);

bool keyboard_shift_pressed(struct keyboard_state *);
bool keyboard_ctrl_pressed(struct keyboard_state *);
bool keyboard_alt_pressed(struct keyboard_state *);
bool keyboard_gui_pressed(struct keyboard_state *);
void keyboard_state_update(struct keyboard_state *, struct key_event *);

#endif
