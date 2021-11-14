#ifndef KEYBOARD_BASE_H_
#define KEYBOARD_BASE_H_

#define KEYBOARD_CLOCK_PIN_NUM 3
#define KEYBOARD_DATA_PIN_NUM 4
#define KEYBOARD_INTERRUPT_NUM 1

#define KEYBOARD_PIN PIND
#define KEYBOARD_PORT PORTD
#define KEYBOARD_INTERRUPT INT1
#define KEYBOARD_INTERRUPT_PREFIX ISC1
#define KEYBOARD_INTERRUPT_CTRL ISC11

#define read_keyboard_data() (KEYBOARD_PIN & (1 << KEYBOARD_DATA_PIN_NUM) ? 1 : 0)
#define setup_keyboard_interrupt() ISR(INT1_vect) { keyboard_interrupt(); }

void keyboard_init(void);

#endif
