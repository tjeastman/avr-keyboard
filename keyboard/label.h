#ifndef AVR_KEYBOARD_LABEL_H_
#define AVR_KEYBOARD_LABEL_H_

struct key_label_map {
  uint8_t value;
  char *label_base;
  char *label_shift;
};

char *key_label(struct keyboard_state *, struct key_event *);

#endif
