#include <stm32f4xx_gpio.h>

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
