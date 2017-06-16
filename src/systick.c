#include <systick.h>

void SysTick_Handler(void)
{
  LEDToggle(GPIO_Pin_13);
}

void SysTick_Init(void)
{
  SysTick_Config(10000000);
  NVIC_EnableIRQ(SysTick_IRQn);
}
