#include <stdio.h>
#include <stdbool.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "usart.h"

#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"
#include "keyboard/label.h"

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
    0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,         // USAGE (Keyboard)
    0xa1, 0x01,         // COLLECTION (Application)
    0x75, 0x01,         //   REPORT_SIZE (1)
    0x95, 0x08,         //   REPORT_COUNT (8)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,         //   USAGE_MINIMUM (Keyboard LeftControl)(224)
    0x29, 0xe7,         //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x01,         //   LOGICAL_MAXIMUM (1)
    0x81, 0x02,         //   INPUT (Data,Var,Abs)
    0x95, 0x01,         //   REPORT_COUNT (1)
    0x75, 0x08,         //   REPORT_SIZE (8)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x65,         //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)(Key Codes)
    0x19, 0x00,         //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,         //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,         //   INPUT (Data,Ary,Abs)
    0xc0                // END_COLLECTION
};

static uchar idle_rate; // repeat rate for keyboards

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;

  struct keyboard_state state;

  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
    switch(rq->bRequest) {
    case USBRQ_HID_GET_REPORT:
      // indicate that no keys are pressed here
      usbMsgPtr = (void *)&state;
      state.modifiers = 0;
      state.values[0] = 0;
      return sizeof(state);
    case USBRQ_HID_SET_REPORT:
      return (rq->wLength.word == 1) ? USB_NO_MSG : 0;
    case USBRQ_HID_GET_IDLE: // send idle rate
      usbMsgPtr = &idle_rate;
      return 1;
    case USBRQ_HID_SET_IDLE: // store idle rate
      idle_rate = rq->wValue.bytes[1];
      return 0;
    }
  }

  return 0;
}

int main(void)
{
  struct keyboard_state state;
  struct key_event event;
  state.modifiers = 0;
  state.values[0] = 0;
  struct scan_code code;

  char input_char;
  struct frame input_frame;

  char *label;

  usart_init();
  stdout = &usart_output;

  while (1) {
    if (usart_getchar(&input_char)) {
      input_frame.data = input_char;
      frame_buffer_insert(input_frame);
    }
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
