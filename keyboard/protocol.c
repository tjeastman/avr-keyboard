#include <stdlib.h>
#include <avr/io.h>
#include "base.h"
#include "protocol.h"

volatile struct frame frame_buffer[FRAME_BUFFER_SIZE];
volatile unsigned int frame_buffer_head = 0;
volatile unsigned int frame_buffer_tail = 0;

volatile struct frame_state state = { FRAME_START, 0 };
volatile struct frame frame;

// avoid volatile qualifier for external consumers of scan codes
struct frame frame_external;

struct frame *frame_buffer_remove(void)
{
  if (frame_buffer_head != frame_buffer_tail) {
    frame_external = frame_buffer[frame_buffer_head];
    frame_buffer_head = frame_buffer_increment(frame_buffer_head);
    return &frame_external;
  }
  return NULL;
}

inline void frame_buffer_insert(volatile struct frame frame)
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

void keyboard_interrupt(void)
{
  if (state.id == FRAME_START) {
    frame.data = 0;
    frame.parity = 0;
  } else if (state.id == FRAME_DATA) {
    frame.data = frame.data >> 1;
    frame.data |= (read_keyboard_data() << 7);
  } else if (state.id ==  FRAME_PARITY) {
    frame.parity = read_keyboard_data();
  } else if (state.id == FRAME_END) {
    frame_buffer_insert(frame);
  }
  frame_state_transition(&state);
}

int is_frame_release(struct frame *frame)
{
  return frame->data == RELEASE_KEY_VALUE;
}

int is_frame_extended(struct frame *frame)
{
  return frame->data == EXTENDED_KEY_VALUE;
}

int is_frame_left_shift(struct frame *frame)
{
  return frame->data == 0x12;
}

int is_frame_right_shift(struct frame *frame)
{
  return frame->data == 0x59;
}

int is_frame_left_ctrl(struct frame *frame)
{
  return frame->data == 0;
}

int is_frame_right_ctrl(struct frame *frame)
{
  return frame->data == 0;
}
