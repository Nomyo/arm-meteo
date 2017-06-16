#ifndef BUTTON_H
#define BUTTON_H

#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

void ButtonInit(void);
void EXTI0_IRQHandler(void);

#endif /* !BUTTON_H */
