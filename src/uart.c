#include <uart.h>

void UART6_config(void)
{
  // Enable clock for GPIOC
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  // Enable clock for USART6
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  // Connect PC6 to USART6_Tx
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  // Connect PC7 to USART6_Rx
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

  // Initialization of GPIOC
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Initialization of USART6
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART6, &USART_InitStruct);

  // Enable USART6
  USART_Cmd(USART6, ENABLE);
}

void UART_putchar(char c)
{
  while (!USART_GetFlagStatus(USART6, USART_FLAG_TXE));
  USART_SendData(USART6, c);
}

void UART_putstring(char *s)
{
  while (*s)
  {
    UART_putchar(*s++);
  }
}

uint16_t UART_getchar(void)
{
  while (!USART_GetFlagStatus(USART6, USART_FLAG_RXNE));
  return USART_ReceiveData(USART6);
}
