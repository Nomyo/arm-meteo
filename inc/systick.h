#ifndef SYSTICK_H
#define SYSTICK_H

#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

void SysTick_Handler(void);
void SysTick_Init(void);

#endif /* !SYSTICK_H */
