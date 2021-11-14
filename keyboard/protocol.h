#ifndef KEYBOARD_PROTOCOL_H_
#define KEYBOARD_PROTOCOL_H_

#define FRAME_BUFFER_SIZE 10

enum frame_state_id
{
  FRAME_START,
  FRAME_DATA,
  FRAME_PARITY,
  FRAME_END
};

struct frame_state
{
  enum frame_state_id id;
  uint8_t nbits; // number of data bits received
};

struct frame
{
  uint8_t start;
  uint8_t value;
  uint8_t parity;
  uint8_t end;
};

typedef uint8_t frame_value_t;

bool frame_buffer_remove(frame_value_t *);

void keyboard_interrupt(void);

#endif
