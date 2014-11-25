#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_buffer *buffer_head;
volatile struct scan_buffer *buffer_tail;
volatile struct scan_buffer *buffer_current;

volatile struct scan_code code;

inline void buffer_insert(volatile struct scan_code code)
{
  buffer_head->code.value = code.value;
  buffer_head->code.nbits = code.nbits;
  buffer_head->code.parity = code.parity;
  buffer_head->code.state = code.state;
  buffer_head = buffer_head->next;
}

inline void scan_code_reset(volatile struct scan_code *code)
{
  code->value = 0;
  code->parity = 0;
  code->nbits = 0;
  code->state = SCAN_START;
}

inline void scan_state_transition(volatile struct scan_code *code)
{
  if (code->state == SCAN_DATA)
    code->nbits += 1;

  if (code->state == SCAN_START)
    code->state = SCAN_DATA;
  else if (code->state == SCAN_DATA && code->nbits < 8)
    code->state = SCAN_DATA;
  else if (code->state == SCAN_DATA && code->nbits == 8)
    code->state = SCAN_PARITY;
  else if (code->state == SCAN_PARITY)
    code->state = SCAN_END;
  else if (code->state == SCAN_END)
    scan_code_reset(code);
}

void keyboard_interrupt(void)
{
  if (code.state == SCAN_DATA) {
    code.value = code.value >> 1;
    if (PIND & (1<<PD3))
      code.value |= 0x80;
  } else if (code.state ==  SCAN_PARITY) {
    code.parity = PIND & (1<<PD3) ? 1 : 0;
  } else if (code.state == SCAN_END) {
    // put the scan code into the buffer
    buffer_insert(code);
  }
  scan_state_transition(&code);
}

void keyboard_init(void)
{
  int i;

  // initialize empty circular linked list buffer of scan codes
  buffer_head = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
  buffer_tail = buffer_head;
  buffer_current = buffer_head;
  for (i = 1; i < BUFFER_SIZE; ++i) {
    buffer_current->next = (struct scan_buffer *)malloc(sizeof(struct scan_buffer));
    buffer_current = buffer_current->next;
  }
  buffer_current->next = buffer_head;

  scan_code_reset(&code);

  // enable pull-up resistors on PD2 and PD3
  PORTD = 1<<PD2 | 1<<PD3;

  // enable INT0 and trigger on falling edge
  EIMSK |= 1<<INT0;
  EICRA |= 1<<ISC01 | 0<<ISC00;
}
