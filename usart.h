#ifndef USART_H
#define USART_H
void uart_init(void);
void my_putchar(char c);
int my_putchar_printf(char c, FILE *stream);
extern FILE uart_output;
#endif
