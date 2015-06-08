#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"

#include "keyboard/base.h"
#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"
#include "keyboard/label.h"

setup_keyboard_interrupt();

int main(void)
{
  frame_value_t value;

  usart_init();
  keyboard_init();

  stdout = &usart_output;

  while (1) {
    if (frame_buffer_valid()) {
      if (frame_buffer_remove(&value)) {
        putchar(value);
      }
    }
    _delay_ms(10);
  }

  return 0;
}
