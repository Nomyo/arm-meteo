#ifndef TIMER_H
#define TIMER_H

#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

void TimerInit(void);
void TIM2_IRQHandler(void);

#endif /* !TIMER_H */
