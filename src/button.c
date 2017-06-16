#include <button.h>

void ButtonInit(void)
{
  // Init GPIO
  GPIO_InitTypeDef GPIO_InitStruct;
  //EXTI structure to init EXT
  EXTI_InitTypeDef EXTI_InitStructure;
  //NVIC structure to set up NVIC controller
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable clock for GPIOA */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable clock for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  //Connect EXTI Line to Button Pin
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  //Configure Button EXTI line
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  //select interrupt mode
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  //generate interrupt on rising edge
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  //enable EXTI line
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  //send values to registers
  EXTI_Init(&EXTI_InitStructure);

  NVIC_EnableIRQ(EXTI0_IRQn); // Enable interrupt
}

void EXTI0_IRQHandler(void)
{
  //Check if EXTI_Line0 is asserted
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    LEDToggle(GPIO_Pin_13);

  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line0);
}
