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
        printf("0x%x [%d] (%d)\r\n", buffer_tail->code.value, buffer_tail->code.value, buffer_tail->code.parity);
        buffer_tail = buffer_tail->next;
      }
    } else {
      printf("Nothing\r\n");
    }
    _delay_ms(4000);
  }

  return 0;
}
