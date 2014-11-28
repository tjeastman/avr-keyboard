#ifndef KEYBOARD_H
#define KEYBOARD_H

#define SCAN_BUFFER_SIZE 10

#define KEYBOARD_CONCAT(a, b) a ## b
#define KEYBOARD_PORT_CODE D
#define KEYBOARD_CLOCK_PIN_NUM 2
#define KEYBOARD_DATA_PIN_NUM 3
#define KEYBOARD_MAKE_PIN(code) KEYBOARD_CONCAT(PIN, code)
#define KEYBOARD_MAKE_PORT(code) KEYBOARD_CONCAT(PORT, code)
#define KEYBOARD_PIN KEYBOARD_MAKE_PIN(KEYBOARD_PORT_CODE)
#define KEYBOARD_PORT KEYBOARD_MAKE_PORT(KEYBOARD_PORT_CODE)
#define KEYBOARD_INTERRUPT_NUM 0
#define KEYBOARD_MAKE_INTERRUPT(num) KEYBOARD_CONCAT(INT, num)
#define KEYBOARD_INTERRUPT KEYBOARD_MAKE_INTERRUPT(KEYBOARD_INTERRUPT_NUM)

#define read_keyboard_data() (KEYBOARD_PIN & (1 << KEYBOARD_DATA_PIN_NUM) ? 1 : 0)

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
  uint8_t nbits; // number of bits received from the device
};

struct scan_code
{
  uint8_t value;
  uint8_t parity;
};

void keyboard_init(void);
void keyboard_interrupt(void);

volatile struct scan_code *scan_buffer_remove(void);

#endif
