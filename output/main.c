#include <stdio.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "usart.h"

// FIX determine a suitable structure for keyboard state data on the device side
struct keyboard_state
{
  uint8_t modifiers;
  uint8_t values[1];
};

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
  usart_init();

  while (1) {
    char c = usart_getchar();
    usart_putchar(c);
  }

  return 0;
}
