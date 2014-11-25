#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "keyboard.h"

ISR(INT0_vect) { keyboard_interrupt(); }

int main(void)
{
  volatile struct scan_code *c;
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  // enable interrupts
  sei();

  printf("Hello\r\n");

  while (1) {
    if (c = buffer_remove()) {
      printf("0x%x [%d] (%d)\r\n", c->value, c->value, c->parity);
    }
    _delay_ms(100);
  }

  return 0;
}
