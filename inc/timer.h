#ifndef TIMER_H
#define TIMER_H

#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <bme280.h>

struct timer_bme280
{
  void (*run)(struct BME280 *param);
  void *param;
};

struct timer_led
{
  void (*run)(unsigned int n);
};

void TimerInit(void);
void TIM2_IRQHandler(void);

#endif /* !TIMER_H */
