#include <stdio.h>

#include "usart.h"

int main(void)
{
  usart_init();

  while (1) {
    char c = usart_getchar();
    usart_putchar(c);
  }

  return 0;
}
