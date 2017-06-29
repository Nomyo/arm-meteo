#ifndef UART_H
#define UART_H

#include <stm32f4xx.h>

void UART6_config(void);
void UART_putchar(char c);
void UART_putstring(char *s);
uint16_t UART_getchar(void);

#endif
