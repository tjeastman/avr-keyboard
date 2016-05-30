#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"

#include "keyboard/protocol.h"
#include "keyboard/base.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"
#include "keyboard/label.h"


setup_keyboard_interrupt();

int main(void)
{
  frame_value_t value;
  struct keyboard_state state;
  struct key_event event;
  state.modifiers = 0;
  state.values[0] = 0;
  struct scan_code code;

  struct frame input_frame;

  char *label;

  usart_init();
  stdout = &usart_output;
  keyboard_init();

  while (true) {
    if (scan_code_read(&code)) {
      printf("Read scan code %d [extended=%d, release=%d]\r\n", code.value, code.extended, code.release);
      if (key_search(&code, &event)) {
        printf("Search success: %d [release=%d]\r\n", event.value, event.release);
        keyboard_state_update(&state, &event);
        if (label = key_label(&state, &event)) {
          printf("Label: %s\r\n", label);
        }

        if (event.release) {
          state.values[0] = 0;
        } else {
          state.values[0] = event.value;
        }
      }
    }
    _delay_ms(10);
  }

  return 0;
}
