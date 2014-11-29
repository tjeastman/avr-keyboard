#ifndef KEYBOARD_H
#define KEYBOARD_H

#define SCAN_BUFFER_SIZE 10

#define RELEASE_KEY_VALUE 0xF0;
#define EXTENDED_KEY_VALUE 0xE0;

#define KEYBOARD_CONCAT(a, b) a ## b
#define KEYBOARD_CONCAT_EXPAND(a, b) KEYBOARD_CONCAT(a, b)

#define KEYBOARD_PORT_CODE D
#define KEYBOARD_CLOCK_PIN_NUM 3
#define KEYBOARD_DATA_PIN_NUM 4
#define KEYBOARD_INTERRUPT_NUM 1

#define KEYBOARD_PIN KEYBOARD_CONCAT_EXPAND(PIN, KEYBOARD_PORT_CODE)
#define KEYBOARD_PORT KEYBOARD_CONCAT_EXPAND(PORT, KEYBOARD_PORT_CODE)
#define KEYBOARD_INTERRUPT KEYBOARD_CONCAT_EXPAND(INT, KEYBOARD_INTERRUPT_NUM)
#define KEYBOARD_INTERRUPT_PREFIX KEYBOARD_CONCAT_EXPAND(ISC, KEYBOARD_INTERRUPT_NUM)
#define KEYBOARD_INTERRUPT_CTRL KEYBOARD_CONCAT_EXPAND(KEYBOARD_INTERRUPT_PREFIX, 1)

#define KEYBOARD_INTERRUPT_SUFFIX KEYBOARD_CONCAT_EXPAND(KEYBOARD_INTERRUPT_NUM, _vect)
#define KEYBOARD_INTERRUPT_VECTOR KEYBOARD_CONCAT_EXPAND(INT, KEYBOARD_INTERRUPT_SUFFIX)

#define read_keyboard_data() (KEYBOARD_PIN & (1 << KEYBOARD_DATA_PIN_NUM) ? 1 : 0)
#define setup_keyboard_interrupt() ISR(KEYBOARD_INTERRUPT_VECTOR) { keyboard_interrupt(); }
#define scan_buffer_increment(name) ((name) + 1) % SCAN_BUFFER_SIZE

enum scan_state_id
{
  SCAN_START,
  SCAN_DATA,
  SCAN_PARITY,
  SCAN_END
};

struct scan_state
{
  enum scan_state_id id;
  uint8_t nbits; // number of bits received from the keyboard
};

struct scan_code
{
  uint8_t value;
  uint8_t parity;
};

void keyboard_init(void);
void keyboard_interrupt(void);

struct scan_code *scan_buffer_remove(void);

int is_code_release(struct scan_code *);
int is_code_extended(struct scan_code *);
int is_code_left_shift(struct scan_code *);
int is_code_right_shift(struct scan_code *);
int is_code_left_ctrl(struct scan_code *);
int is_code_right_ctrl(struct scan_code *);

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
  uint8_t release_mode;
  uint8_t extended_mode;
  uint8_t final;
};

int keyboard_shift_pressed(struct keyboard_state *);
int keyboard_ctrl_pressed(struct keyboard_state *);
void keyboard_state_transition(struct keyboard_state *, struct scan_code *);

#endif
