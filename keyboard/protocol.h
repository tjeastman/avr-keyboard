#ifndef AVR_KEYBOARD_PROTOCOL_H_
#define AVR_KEYBOARD_PROTOCOL_H_

#define FRAME_BUFFER_SIZE 10

#define RELEASE_KEY_VALUE 0xF0;
#define EXTENDED_KEY_VALUE 0xE0;

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

struct frame *frame_buffer_remove(void);

int is_frame_release(struct frame *);
int is_frame_extended(struct frame *);
int is_frame_left_shift(struct frame *);
int is_frame_right_shift(struct frame *);
int is_frame_left_ctrl(struct frame *);
int is_frame_right_ctrl(struct frame *);

void keyboard_interrupt(void);

#endif
