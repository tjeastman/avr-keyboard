#ifndef AVR_KEYBOARD_PROTOCOL_H_
#define AVR_KEYBOARD_PROTOCOL_H_

#define FRAME_BUFFER_SIZE 10

#define frame_buffer_increment(name) ((name) + 1) % FRAME_BUFFER_SIZE

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
  uint8_t data;
  uint8_t parity;
  uint8_t end;
};

typedef uint8_t frame_value_t;

int frame_buffer_valid(void);
uint8_t frame_buffer_remove(void);

void keyboard_interrupt(void);

#endif
