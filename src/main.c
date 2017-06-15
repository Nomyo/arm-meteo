#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

#include <i2c1.h>
#include <misc.h>

#include <time.h>

#undef errno
extern int errno;

void _exit(int ret)
{
  while (1);
}

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

void EXTI0_IRQHandler(void)
{
  //Check if EXTI_Line0 is asserted
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    LEDToggle(GPIO_Pin_13);
  }

  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line0);
}

void SysTick_Handler(void)
{
  LEDToggle(GPIO_Pin_13);
}

void SysTick_Init(void)
{
  SysTick_Config(10000000);
  NVIC_EnableIRQ(SysTick_IRQn);
}

int main()
{
  LEDsInit();
  ButtonInit();

  timer_t timer;
  //TimerInit(&timer);


  //SysTick_Init();

  init_I2C1(); // initialize I2C peripheral

  uint8_t received_data[2];

  while(1)
  {
    // start a transmission in Master transmitter mode
    I2C_start(I2C1, 0x77 << 1, I2C_Direction_Transmitter);
    // write one byte to the slave
    I2C_write(I2C1, 0xD0); // CHIP REGISTER => 0x60
    // stop the transmission
    I2C_stop(I2C1);
    // start a transmission in Master receiver mode
    I2C_start(I2C1, 0x77 << 1, I2C_Direction_Receiver);
    // read one byte and request another byte
    received_data[0] = I2C_read_nack(I2C1);
  }

  while (1);
}

void _start(void)
{
  main();
}


void __attribute__((weak)) dummy_handler()
{
  return;
}

__attribute__((section (".isr_vector"))) const int isr_table[] =
{
  0x20002000,
  (int) _start,
  (int) dummy_handler,                   //Hard fault
  (int) dummy_handler,                   //NonMaskableInt_IRQn         = -14,
  (int) dummy_handler,                   //MemoryManagement_IRQn       = -12,
  (int) dummy_handler,                   //BusFault_IRQn               = -11,
  (int) dummy_handler,                   //UsageFault_IRQn             = -10,
  0,                                     //reserved
  0,                                     //reserved
  0,                                     //reserved
  0,                                     //reserved
  (int) dummy_handler,                   //SVCall_IRQn                 = -5,
  (int) dummy_handler,                   //DebugMonitor_IRQn           = -4,
  0,                                     //reserved
  (int) dummy_handler,                   //PendSV_IRQn                 = -2,
  (int) SysTick_Handler,                 //SysTick_IRQn                = -1,
  (int) dummy_handler,                   //WWDG_IRQn                   = 0,
  (int) dummy_handler,                   //PVD_IRQn                    = 1,
  (int) dummy_handler,                   //TAMP_STAMP_IRQn             = 2,
  (int) dummy_handler,                   //RTC_WKUP_IRQn               = 3,
  (int) dummy_handler,                   //FLASH_IRQn                  = 4,
  (int) dummy_handler,                   //RCC_IRQn                    = 5,
  (int) EXTI0_IRQHandler,                //EXTI0_IRQn                  = 6,
  (int) dummy_handler,                   //EXTI1_IRQn                  = 7,
  (int) dummy_handler,                   //EXTI2_IRQn                  = 8,
  (int) dummy_handler,                   //EXTI3_IRQn                  = 9,
  (int) dummy_handler,                   //EXTI4_IRQn                  = 10,
  (int) dummy_handler,                   //DMA1_Stream0_IRQn           = 11,
  (int) dummy_handler,                   //DMA1_Stream1_IRQn           = 12,
  (int) dummy_handler,                   //DMA1_Stream2_IRQn           = 13,
  (int) dummy_handler,                   //DMA1_Stream3_IRQn           = 14,
  (int) dummy_handler,                   //DMA1_Stream4_IRQn           = 15,
  (int) dummy_handler,                   //DMA1_Stream5_IRQn           = 16,
  (int) dummy_handler,                   //DMA1_Stream6_IRQn           = 17,
  (int) dummy_handler,                   //ADC_IRQn                    = 18,
  (int) dummy_handler,                   //CAN1_TX_IRQn                = 19,
  (int) dummy_handler,                   //CAN1_RX0_IRQn               = 20,
  (int) dummy_handler,                   //CAN1_RX1_IRQn               = 21,
  (int) dummy_handler,                   //CAN1_SCE_IRQn               = 22,
  (int) dummy_handler,                   //EXTI9_5_IRQn                = 23,
  (int) dummy_handler,                   //TIM1_BRK_TIM9_IRQn          = 24,
  (int) dummy_handler,                   //TIM1_UP_TIM10_IRQn          = 25,
  (int) dummy_handler,                   //TIM1_TRG_COM_TIM11_IRQn     = 26,
  (int) dummy_handler,                   //TIM1_CC_IRQn                = 27,
  (int) dummy_handler,                   //TIM2_IRQn                   = 28,
  (int) dummy_handler,                   //TIM3_IRQn                   = 29,
  (int) dummy_handler,                   //TIM4_IRQn                   = 30,
  (int) dummy_handler,                   //I2C1_EV_IRQn                = 31,
  (int) dummy_handler,                   //I2C1_ER_IRQn                = 32,
  (int) dummy_handler,                   //I2C2_EV_IRQn                = 33,
  (int) dummy_handler,                   //I2C2_ER_IRQn                = 34,
  (int) dummy_handler,                   //SPI1_IRQn                   = 35,
  (int) dummy_handler,                   //SPI2_IRQn                   = 36,
  (int) dummy_handler,                   //USART1_IRQn                 = 37,
  (int) dummy_handler,                   //USART2_IRQn                 = 38,
  (int) dummy_handler,                   //USART3_IRQn                 = 39,
  (int) dummy_handler,                   //EXTI15_10_IRQn              = 40,
  (int) dummy_handler,                   //RTC_Alarm_IRQn              = 41,
  (int) dummy_handler,                   //OTG_FS_WKUP_IRQn            = 42,
  (int) dummy_handler,                   //TIM8_BRK_TIM12_IRQn         = 43,
  (int) dummy_handler,                   //TIM8_UP_TIM13_IRQn          = 44,
  (int) dummy_handler,                   //TIM8_TRG_COM_TIM14_IRQn     = 45,
  (int) dummy_handler,                   //TIM8_CC_IRQn                = 46,
  (int) dummy_handler,                   //DMA1_Stream7_IRQn           = 47,
  (int) dummy_handler,                   //FMC_IRQn                    = 48,
  (int) dummy_handler,                   //SDIO_IRQn                   = 49,
  (int) dummy_handler,                   //TIM5_IRQn                   = 50,
  (int) dummy_handler,                   //SPI3_IRQn                   = 51,
  (int) dummy_handler,                   //UART4_IRQn                  = 52,
  (int) dummy_handler,                   //UART5_IRQn                  = 53,
  (int) dummy_handler,                   //TIM6_DAC_IRQn               = 54,
  (int) dummy_handler,                   //TIM7_IRQn                   = 55,
  (int) dummy_handler,                   //DMA2_Stream0_IRQn           = 56,
  (int) dummy_handler,                   //DMA2_Stream1_IRQn           = 57,
  (int) dummy_handler,                   //DMA2_Stream2_IRQn           = 58,
  (int) dummy_handler,                   //DMA2_Stream3_IRQn           = 59,
  (int) dummy_handler,                   //DMA2_Stream4_IRQn           = 60,
  (int) dummy_handler,                   //ETH_IRQn                    = 61,
  (int) dummy_handler,                   //ETH_WKUP_IRQn               = 62,
  (int) dummy_handler,                   //CAN2_TX_IRQn                = 63,
  (int) dummy_handler,                   //CAN2_RX0_IRQn               = 64,
  (int) dummy_handler,                   //CAN2_RX1_IRQn               = 65,
  (int) dummy_handler,                   //CAN2_SCE_IRQn               = 66,
  (int) dummy_handler,                   //OTG_FS_IRQn                 = 67,
  (int) dummy_handler,                   //DMA2_Stream5_IRQn           = 68,
  (int) dummy_handler,                   //DMA2_Stream6_IRQn           = 69,
  (int) dummy_handler,                   //DMA2_Stream7_IRQn           = 70,
  (int) dummy_handler,                   //USART6_IRQn                 = 71,
  (int) dummy_handler,                   //I2C3_EV_IRQn                = 72,
  (int) dummy_handler,                   //I2C3_ER_IRQn                = 73,
  (int) dummy_handler,                   //OTG_HS_EP1_OUT_IRQn         = 74,
  (int) dummy_handler,                   //OTG_HS_EP1_IN_IRQn          = 75,
  (int) dummy_handler,                   //OTG_HS_WKUP_IRQn            = 76,
  (int) dummy_handler,                   //OTG_HS_IRQn                 = 77,
  (int) dummy_handler,                   //DCMI_IRQn                   = 78,
  (int) dummy_handler,                   //CRYP_IRQn                   = 79,
  (int) dummy_handler,                   //HASH_RNG_IRQn               = 80,
  (int) dummy_handler,                   //FPU_IRQn                    = 81,
  (int) dummy_handler,                   //UART7_IRQn                  = 82,
  (int) dummy_handler,                   //UART8_IRQn                  = 83,
  (int) dummy_handler,                   //SPI4_IRQn                   = 84,
  (int) dummy_handler,                   //SPI5_IRQn                   = 85,
  (int) dummy_handler,                   //SPI6_IRQn                   = 86,
  (int) dummy_handler,                   //SAI1_IRQn                   = 87,
  (int) dummy_handler,                   //LTDC_IRQn                   = 88,
  (int) dummy_handler,                   //LTDC_ER_IRQn                = 89,
  (int) dummy_handler,                   //DMA2D_IRQn                  = 90
};
