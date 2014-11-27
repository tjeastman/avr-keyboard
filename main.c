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
    {"a", 0x1C},
    {"b", 0x32},
    {"c", 0x21},
    {"d", 0x23},
    {"e", 0x24},
    {"f", 0x2B},
    {"g", 0x34},
    {"h", 0x33},
    {"i", 0x43},
    {"j", 0x3B},
    {"k", 0x42},
    {"l", 0x4B},
    {"m", 0x3A},
    {"n", 0x31},
    {"o", 0x44},
    {"p", 0x4D},
    {"q", 0x15},
    {"r", 0x2D},
    {"s", 0x1B},
    {"t", 0x2C},
    {"u", 0x3C},
    {"v", 0x2A},
    {"w", 0x1D},
    {"x", 0x22},
    {"y", 0x35},
    {"z", 0x1A},
    {"0", 0x45},
    {"1", 0x16},
    {"2", 0x1E},
    {"3", 0x26},
    {"4", 0x25},
    {"5", 0x2E},
    {"6", 0x36},
    {"7", 0x3D},
    {"8", 0x3E},
    {"9", 0x46},
    {"[F1]", 0x05},
    {"[F2]", 0x06},
    {"[F3]", 0x04},
    {"[F4]", 0x0C},
    {"[F5]", 0x03},
    {"[F6]", 0x0B},
    {"[F7]", 0x83},
    {"[F8]", 0x0A},
    {"[F9]", 0x01},
    {"[F10]", 0x09},
    {"[F11]", 0x78},
    {"[F12]", 0x07},
    {"[ESC]", 0x76},
    {"`", 0x0E},
    {"-", 0x4E},
    {"\\", 0x5D},
    {"[", 0x54},
    {"]", 0x5B},
    {";", 0x4C},
    {",", 0x41},
    {".", 0x49},
    {"/", 0x4A},
    {"\r\n", 0x5A},
    {" ", 0x29}
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

  for (int i = 0; i < 60; ++i) {
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
