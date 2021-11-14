#ifndef USART_H
#define USART_H
void usart_init(void);
int usart_putchar(char c, FILE *stream);
extern FILE usart_output;
#endif
