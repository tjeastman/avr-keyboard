#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "keyboard.h"

ISR(INT0_vect) { keyboard_interrupt(); }

int main(void) {
  int i, size, item;
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  // enable interrupts
  sei();

  while (1) {
    size = get_buffer_size();
    for (i = 0; i < size; ++i) {
      item = get_buffer_item(i);
      printf("0x%x [%d] ", item, item);
    }
    if (size > 0) {
      printf("\r\n");
    }
    reset_buffer();
    _delay_ms(4000);
  }

  return 0;
}
