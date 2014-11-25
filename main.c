#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "keyboard.h"

ISR(INT0_vect) { keyboard_interrupt(); }

int main(void)
{
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  // enable interrupts
  sei();

  printf("Hello\r\n");

  while (1) {
    if (buffer_tail != buffer_head) {
      printf("0x%x [%d] (%d)\r\n", buffer_tail->code.value, buffer_tail->code.value, buffer_tail->code.parity);
      buffer_tail = buffer_tail->next;
    }
    _delay_ms(100);
  }

  return 0;
}
