#include <avr/io.h>
#include <stdlib.h>
#include "keyboard.h"

volatile struct scan_buffer *buffer_head;
volatile struct scan_buffer *buffer_tail;
volatile struct scan_buffer *buffer_current;

volatile enum scan_state state = SCAN_START;
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

inline enum scan_state scan_state_transition(struct scan_code code)
{
  if (code.state == SCAN_START)
    return SCAN_DATA;
  else if (code.state == SCAN_DATA && code.nbits < 8)
    return SCAN_DATA;
  else if (code.state == SCAN_DATA && code.nbits == 8)
    return SCAN_PARITY;
  else if (code.state == SCAN_PARITY)
    return SCAN_END;
  else if (code.state == SCAN_END)
    return SCAN_START;

  return code.state;
}

void keyboard_interrupt(void)
{
  switch (code.state) {
  case SCAN_START:
    code.state = scan_state_transition(code);
    break;
  case SCAN_DATA:
    code.value = code.value >> 1;
    if (PIND & (1<<PD3))
      code.value |= 0x80;
    code.nbits += 1;
    code.state = scan_state_transition(code);
    break;
  case SCAN_PARITY:
    code.state = scan_state_transition(code);
    code.parity = PIND & (1<<PD3) ? 1 : 0;
    break;
  case SCAN_END:
    // put the scan code into the buffer
    buffer_insert(code);
    scan_code_reset(&code);
    break;
  default:
    break;
  }
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
