#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "keyboard.h"

ISR(INT0_vect) { keyboard_interrupt(); }

struct key {
  char *label;
  uint8_t value;
};

uint8_t RELEASE_KEY_VALUE = 0xF0;
uint8_t EXTENDED_KEY_VALUE = 0xE0;
uint8_t LEFT_SHIFT_VALUE = 0x12;
uint8_t RIGHT_SHIFT_VALUE = 0x59;

struct key keys[] =
  {
    {"[F9]", 0x01},
    {"[F5]", 0x03},
    {"[F3]", 0x04},
    {"[F1]", 0x05},
    {"[F2]", 0x06},
    {"[F12]", 0x07},
    {"[F10]", 0x09},
    {"[F8]", 0x0A},
    {"[F6]", 0x0B},
    {"[F4]", 0x0C},
    {"\t", 0x0D},
    {"`", 0x0E},
    {"q", 0x15},
    {"1", 0x16},
    {"z", 0x1A},
    {"s", 0x1B},
    {"a", 0x1C},
    {"w", 0x1D},
    {"2", 0x1E},
    {"c", 0x21},
    {"x", 0x22},
    {"d", 0x23},
    {"e", 0x24},
    {"4", 0x25},
    {"3", 0x26},
    {" ", 0x29},
    {"v", 0x2A},
    {"f", 0x2B},
    {"t", 0x2C},
    {"r", 0x2D},
    {"5", 0x2E},
    {"n", 0x31},
    {"b", 0x32},
    {"h", 0x33},
    {"g", 0x34},
    {"y", 0x35},
    {"6", 0x36},
    {"m", 0x3A},
    {"j", 0x3B},
    {"u", 0x3C},
    {"7", 0x3D},
    {"8", 0x3E},
    {",", 0x41},
    {"k", 0x42},
    {"i", 0x43},
    {"o", 0x44},
    {"0", 0x45},
    {"9", 0x46},
    {".", 0x49},
    {"/", 0x4A},
    {"l", 0x4B},
    {";", 0x4C},
    {"p", 0x4D},
    {"-", 0x4E},
    {"'", 0x52},
    {"[", 0x54},
    {"=", 0x55},
    {"[CAPS]", 0x58},
    {"\r\n", 0x5A},
    {"]", 0x5B},
    {"\\", 0x5D},
    {"[BACKSPACE]", 0x66},
    {"[SCROLL]", 0x7E},
    {"[ESC]", 0x76},
    {"[NUM]", 0x77},
    {"[F11]", 0x78},
    {"[F7]", 0x83}
  };

char *decode(volatile struct scan_code *c)
{
  static uint8_t shift_key_pressed = 0;
  static uint8_t release_key_pressed = 0;

  if (release_key_pressed) {
    release_key_pressed = 0;
    return NULL;
  } else if (c->value == RELEASE_KEY_VALUE) {
    release_key_pressed = 1;
    return NULL;
  }

  for (int i = 0; i < 67; ++i) {
    if (keys[i].value == c->value) {
      return keys[i].label;
    }
  }
  return NULL;
}

int main(void)
{
  volatile struct scan_code *code;
  char *label;
  usart_init();
  keyboard_init();

  stdout = &usart_output;

  // enable interrupts
  sei();

  printf("Hello [%d]\r\n", sizeof(keys));

  while (1) {
    if (code = scan_buffer_remove()) {
      if (label = decode(code)) {
        printf("%s", label);
      }
    }
    _delay_ms(10);
  }

  return 0;
}
