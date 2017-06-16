#include <led.h>

void LEDToggle(unsigned int LEDn)
{
  if(GPIO_ReadOutputDataBit(GPIOG, LEDn))  //read previous state
  {
    GPIO_ResetBits(GPIOG, LEDn); //set to zero
  }
  else
  {
    GPIO_SetBits(GPIOG, LEDn);//set to one
  }
}

void LEDsInit(void)
{
  //GPIO structure used to initialize LED port
  GPIO_InitTypeDef GPIO_InitDef;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
  GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;

  //Initialize pins
  GPIO_Init(GPIOG, &GPIO_InitDef);
}
