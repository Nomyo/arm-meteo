#ifndef UART_H
#define UART_H

#include <stm32f4xx.h>
#include <bme280.h>

void UART6_config(void);
void UART_putchar(char c);
void UART_putstring(char *s);
uint16_t UART_getchar(void);
void swap(char *str1, char *str2);
void reverse(char str[], int len);
void itoa(char* str, int num);
void fill_msg1(char *msg, struct BME280* bme);
void fill_msg2(char *msg, struct BME280 *bme);
void fill_msg3(char *msg, struct BME280 *bme);

#endif
