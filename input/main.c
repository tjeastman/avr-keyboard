#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"

#include "keyboard/base.h"
#include "keyboard/protocol.h"

setup_keyboard_interrupt();

int main(void)
{
  frame_value_t value;

  usart_init();
  keyboard_init();

  while (1) {
    if (frame_buffer_valid()) {
      if (frame_buffer_remove(&value)) {
        usart_putchar(value);
      }
    }
    _delay_ms(10);
  }

  return 0;
}
