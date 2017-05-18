#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <misc.h>

#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>


#undef errno
extern int errno;

void _exit(int ret)
{
  while (1);
}

/*
void Configure_PA0(void)
{
  EXTI_InitTypeDef EXTI_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;

  // BUTTON
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStruct2;

  GPIO_InitStruct2.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct2.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct2.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct2.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct2);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_Init(&EXTI_InitStruct);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(void) {
  if (GPIOA->IDR & 0x0001)
  {
    GPIO_SetBits(GPIOG, GPIO_Pin_13);
  }
  else
  {
    GPIO_ResetBits(GPIOG, GPIO_Pin_13);
  }
}
*/
int main(void)
{
  // stm32f4xx_rcc.h
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  // LED
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;

  GPIO_Init(GPIOG, &GPIO_InitStruct);


  //Configure_PA0();

  while (1) {
  }

}

void _start(void)
{
  main();
}

__attribute__((section (".isr_vector"))) const int isr_table[] =
{
  0x20002000,
  (int) _start
};
