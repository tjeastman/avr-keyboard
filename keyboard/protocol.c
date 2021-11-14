#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>

#include "keyboard/base.h"
#include "keyboard/protocol.h"

volatile struct frame frame_buffer[FRAME_BUFFER_SIZE];
volatile unsigned int frame_buffer_head = 0;
volatile unsigned int frame_buffer_tail = 0;

volatile struct frame_state state = { FRAME_START, 0 };
volatile struct frame frame;

bool frame_buffer_remove(frame_value_t *frame_value)
{
  if (frame_buffer_head == frame_buffer_tail) {
    return false;
  }
  *frame_value = frame_buffer[frame_buffer_head].value;
  frame_buffer_head += 1;
  frame_buffer_head %= FRAME_BUFFER_SIZE;
  return true;
}

inline void frame_buffer_insert(volatile struct frame frame)
{
  frame_buffer[frame_buffer_tail] = frame;
  frame_buffer_tail += 1;
  frame_buffer_tail %= FRAME_BUFFER_SIZE;
}

inline void frame_state_transition(volatile struct frame_state *state)
{
  if (state->id == FRAME_DATA) {
    state->nbits += 1;
  }

  if (state->id == FRAME_START) {
    state->id = FRAME_DATA;
    state->nbits = 0;
  } else if (state->id == FRAME_DATA && state->nbits < 8) {
    state->id = FRAME_DATA;
  } else if (state->id == FRAME_DATA && state->nbits == 8) {
    state->id = FRAME_PARITY;
  } else if (state->id == FRAME_PARITY) {
    state->id = FRAME_END;
  } else if (state->id == FRAME_END) {
    state->id = FRAME_START;
  }
}

void keyboard_interrupt(void)
{
  if (state.id == FRAME_START) {
    frame.value = 0;
    frame.parity = 0;
  } else if (state.id == FRAME_DATA) {
    frame.value = frame.value >> 1;
    frame.value |= (read_keyboard_data() << 7);
  } else if (state.id ==  FRAME_PARITY) {
    frame.parity = read_keyboard_data();
  } else if (state.id == FRAME_END) {
    frame_buffer_insert(frame);
  }
  frame_state_transition(&state);
}
