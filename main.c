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

  printf("Hello\r\n");

  while (1) {
    if (buffer_tail != buffer_head) {
      if (buffer_tail->state == SCAN_END && buffer_tail->code.nbits == 8) {
        printf("0x%x [%d] (%d / %d)\r\n", buffer_tail->code.value, buffer_tail->code.value, buffer_tail->code.parity, buffer_tail->code.nbits);
        buffer_tail = buffer_tail->next;
      }
    }
    _delay_ms(100);
  }

  return 0;
}
