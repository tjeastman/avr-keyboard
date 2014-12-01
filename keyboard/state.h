#ifndef AVR_KEYBOARD_STATE_H_
#define AVR_KEYBOARD_STATE_H_

struct key {
  uint8_t value;
  char *label_base;
  char *label_shift;
};

struct key_map {
  struct scan_code code;
  struct key key;
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

int is_key_left_shift(struct key *);
int is_key_right_shift(struct key *);
int is_key_left_ctrl(struct key *);
int is_key_right_ctrl(struct key *);
int is_key_left_alt(struct key *);
int is_key_right_alt(struct key *);
int is_key_left_gui(struct key *);
int is_key_right_gui(struct key *);

int key_compare(const void *, const void *);
int key_search(struct scan_code *, struct key *);
char *keyboard_state_label(struct keyboard_state, struct key *);

int keyboard_shift_pressed(struct keyboard_state *);
int keyboard_ctrl_pressed(struct keyboard_state *);
int keyboard_alt_pressed(struct keyboard_state *);
int keyboard_gui_pressed(struct keyboard_state *);
void keyboard_state_update(struct keyboard_state *, struct scan_code *, struct key *);

#endif
