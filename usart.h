#ifndef USART_H
#define USART_H
void usart_init(void);
void usart_putchar(char c);
int usart_putchar_printf(char c, FILE *stream);
extern FILE usart_output;
#endif
