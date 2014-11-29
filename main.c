#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "usbdrv.h"
#include "usart.h"
#include "keyboard.h"

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

struct key {
  uint8_t value;
  uint8_t value_usb;
  char *label;
  char *label_shift;
};

struct key_page {
  struct key *keys;
  unsigned int size;
};

struct key keys[] =
  {
    {0x01, 0x42, "[F9]", NULL},
    {0x03, 0x3E, "[F5]", NULL},
    {0x04, 0x3C, "[F3]", NULL},
    {0x05, 0x3A, "[F1]", NULL},
    {0x06, 0x3B, "[F2]", NULL},
    {0x07, 0x45, "[F12]", NULL},
    {0x09, 0x43, "[F10]", NULL},
    {0x0A, 0x41, "[F8]", NULL},
    {0x0B, 0x3F, "[F6]", NULL},
    {0x0C, 0x3D, "[F4]", NULL},
    {0x0D, 0x2B, "\t", NULL},
    {0x0E, 0x35, "`", "~"},
    // 0x11, 0xE2, Left Alt
    // 0x12, 0xE1, Left Shift
    // 0x14, 0xE0,  Left Ctrl
    {0x15, 0x14, "q", "Q"},
    {0x16, 0x1E, "1", "!"},
    {0x1A, 0x1D, "z", "Z"},
    {0x1B, 0x16, "s", "S"},
    {0x1C, 0x04, "a", "A"},
    {0x1D, 0x1A, "w", "W"},
    {0x1E, 0x1F, "2", "@"},
    {0x21, 0x06, "c", "C"},
    {0x22, 0x1B, "x", "X"},
    {0x23, 0x07, "d", "D"},
    {0x24, 0x08, "e", "E"},
    {0x25, 0x21, "4", "$"},
    {0x26, 0x20, "3", "#"},
    {0x29, 0x2C, " ", NULL},
    {0x2A, 0x19, "v", "V"},
    {0x2B, 0x09, "f", "F"},
    {0x2C, 0x17, "t", "T"},
    {0x2D, 0x15, "r", "R"},
    {0x2E, 0x22, "5", "%"},
    {0x31, 0x11, "n", "N"},
    {0x32, 0x05, "b", "B"},
    {0x33, 0x0B, "h", "H"},
    {0x34, 0x0A, "g", "G"},
    {0x35, 0x1C, "y", "Y"},
    {0x36, 0x23, "6", "^"},
    {0x3A, 0x10, "m", "M"},
    {0x3B, 0x0D, "j", "J"},
    {0x3C, 0x18, "u", "U"},
    {0x3D, 0x24, "7", "&"},
    {0x3E, 0x25, "8", "*"},
    {0x41, 0x36, ",", "<"},
    {0x42, 0x0E, "k", "K"},
    {0x43, 0x0C, "i", "I"},
    {0x44, 0x12, "o", "O"},
    {0x45, 0x27, "0", ")"},
    {0x46, 0x26, "9", "("},
    {0x49, 0x37, ".", ">"},
    {0x4A, 0x38, "/", "?"},
    {0x4B, 0x0F, "l", "L"},
    {0x4C, 0x33, ";", ":"},
    {0x4D, 0x13, "p", "P"},
    {0x4E, 0x2D, "-", "_"},
    {0x52, 0x34, "'", "\""},
    {0x54, 0x2F, "[", "{"},
    {0x55, 0x2E, "=", "+"},
    {0x58, 0x39, "[CAPS]", NULL},
    // 0x59, 0xE5, Right Shift
    {0x5A, 0x28, "\r\n", NULL},
    {0x5B, 0x30, "]", "}"},
    {0x5D, 0x31, "\\", "|"},
    {0x66, 0x2A, "[BACKSPACE]", NULL},
    {0x76, 0x29, "[ESC]", NULL},
    {0x77, 0x53, "[NUM]", NULL},
    {0x78, 0x44, "[F11]", NULL},
    {0x7E, 0x47, "[SCROLL]", NULL},
    {0x83, 0x40, "[F7]", NULL}
  };

struct key extended_keys[] =
  {
    {0x69, 0x00, "[END]", NULL},
    {0x6B, 0x00, "[LEFT]", NULL},
    {0x6C, 0x00, "[HOME]", NULL},
    {0x70, 0x00, "[INS]", NULL},
    {0x71, 0x00, "[DEL]", NULL},
    {0x72, 0x00, "[DOWN]", NULL},
    {0x74, 0x00, "[RIGHT]", NULL},
    {0x75, 0x00, "[UP]", NULL},
    {0x7A, 0x00, "[PGDOWN]", NULL},
    {0x7D, 0x00, "[PGUP]", NULL}
  };

struct key_page default_key_page = { keys, 67 };
struct key_page extended_key_page = { extended_keys, 10 };

int compare_keys(const void *k1, const void *k2)
{
  struct key *key1 = (struct key *)k1;
  struct key *key2 = (struct key *)k2;
  return key1->value - key2->value;
}

struct key *lookup_key(struct scan_code *code, struct key_page *current_keys)
{
  struct key search_key;
  search_key.value = code->value;
  return bsearch(&search_key, current_keys->keys, current_keys->size, sizeof(struct key), compare_keys);
}

char *decode(struct scan_code *code)
{
  static struct keyboard_state state = { 0, 0 };
  static struct key_page *current_keys = &default_key_page;
  struct key *found_key;
  char *label = NULL;

  keyboard_state_transition(&state, code);

  if (is_code_extended(code)) {
    // swap in a new "page" of scan codes
    current_keys = &extended_key_page;
    return NULL;
  }

  if (state.release_mode) {
    state.release_mode = 0;
  } else if (is_code_release(code)) {
    state.release_mode = 1;
  } else if (keyboard_shift_pressed(&state)) {
    found_key = lookup_key(code, current_keys);
    if (found_key) {
      if (found_key->label_shift) {
        label = found_key->label_shift;
      } else {
        label = found_key->label;
      }
    }
  } else {
    found_key = lookup_key(code, current_keys);
    if (found_key) {
      label = found_key->label;
    }
  }

  // switch back to the default "page" of scan codes
  current_keys = &default_key_page;

  return label;
}

USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
  return 0;
}

int main(void)
{
  struct scan_code *code;
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

  printf("Hello [%d]\r\n", sizeof(keys));

  while (1) {
    wdt_reset(); // reset the watchdog timer
    usbPoll();
    if (code = scan_buffer_remove()) {
      if (label = decode(code)) {
        printf("%s", label);
      }
    }
    _delay_ms(10);
  }

  return 0;
}
