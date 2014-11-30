#ifndef AVR_KEYBOARD_STATE_H_
#define AVR_KEYBOARD_STATE_H_

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
};

int keyboard_shift_pressed(struct keyboard_state *);
int keyboard_ctrl_pressed(struct keyboard_state *);
void keyboard_state_transition(struct keyboard_state *, struct scan_code *);

#endif
