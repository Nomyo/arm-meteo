#ifndef LED_H
#define LED_H

#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

void LEDToggle(unsigned int Ledn);
void LEDsInit(void);

#endif /* !LED_H */
