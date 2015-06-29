#include <stdlib.h>
#include <avr/io.h>

#include "keyboard/base.h"
#include "keyboard/protocol.h"

volatile struct frame frame_buffer[FRAME_BUFFER_SIZE];
volatile unsigned int frame_buffer_head = 0;
volatile unsigned int frame_buffer_tail = 0;

volatile struct frame_state state = { FRAME_START, 0 };
volatile struct frame frame;

int frame_buffer_valid(void)
{
  return frame_buffer_head != frame_buffer_tail;
}

int frame_buffer_remove(frame_value_t *value)
{
  if (frame_buffer_head != frame_buffer_tail) {
    *value = frame_buffer[frame_buffer_head].data;
    frame_buffer_head = frame_buffer_increment(frame_buffer_head);
    return 1;
  } else {
    return 0;
  }
}

void frame_buffer_insert(struct frame frame)
{
  frame_buffer[frame_buffer_tail] = frame;
  frame_buffer_tail = frame_buffer_increment(frame_buffer_tail);
}

inline void frame_state_transition(volatile struct frame_state *state)
{
  if (state->id == FRAME_DATA)
    state->nbits += 1;

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
