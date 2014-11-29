#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "base.h"

void keyboard_init(void)
{
  // enable pull-up resistors
  KEYBOARD_PORT |= _BV(KEYBOARD_CLOCK_PIN_NUM) | _BV(KEYBOARD_DATA_PIN_NUM);

  // enable the keyboard interrupt
  EIMSK |= _BV(KEYBOARD_INTERRUPT);
  // trigger the keyboard interrupt on the falling edge
  EICRA |= _BV(KEYBOARD_INTERRUPT_CTRL);

  // enable interrupts
  sei();
}
