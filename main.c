#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "usart.h"

#include "keyboard/base.h"
#include "keyboard/protocol.h"
#include "keyboard/scan.h"
#include "keyboard/state.h"

setup_keyboard_interrupt();

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
  state.modifiers = 0;
  state.values[0] = 0;
  struct scan_code code;
  struct key *key;

  char *label;

  wdt_enable(WDTO_1S); // enable 1s watchdog timer

  usbInit();

  usbDeviceDisconnect(); // enforce re-enumeration
  for (int i = 0; i < 250; ++i) {
    wdt_reset(); // reset the watchdog timer
    _delay_ms(2);
  }
  usbDeviceConnect();

  usart_init();
  keyboard_init();

  stdout = &usart_output;

  printf("Hello\r\n");

  while (1) {
    wdt_reset(); // reset the watchdog timer
    usbPoll();
    if (scan_code_read(&code)) {
      keyboard_state_update(&state, &code);
      if (key = key_search(&code)) {
        if (!code.release) {
          if (label = keyboard_state_label(state, key)) {
            printf("%s", label);
          }
        }

        if (code.release) {
          state.values[0] = 0;
        } else if (key->value_usb == 0) {
          state.values[0] = 0;
        } else {
          state.values[0] = key->value_usb;
        }
        //usbSetInterrupt((void *)&state, sizeof(state));
      }
    }
    _delay_ms(10);
  }

  return 0;
}
