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

void UART_printbme(struct BME280 *bme)
{
  char msg1[50];
  fill_msg1(msg1, bme);
  UART_putstring(msg1);

  char *cel = "°C\r\n";
  UART_putstring(cel);

  char msg2[30];
  fill_msg2(msg2, bme);
  UART_putstring(msg2);

  char *pre = "hpa\r\n";
  UART_putstring(pre);

  char msg3[30];
  fill_msg3(msg3, bme);
  UART_putstring(msg3);

  char *perc = "%\r\n\r\n";
  UART_putstring(perc);
}

void swap(char *str1, char *str2)
{
  char c = *str1;
  *str1 = *str2;
  *str2 = c;
}

void reverse(char str[], int len)
{
  int start = 0;
  int end = len - 1;
  while (start < end)
  {
    swap(str + start, str + end);
    start++;
    end--;
  }
}

void itoa(char* str, int num)
{
  int i = 0;
  int isNegative = 0;

  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
  }

  if (num < 0)
  {
    isNegative = 1;
    num = -num;
  }

  // Process individual digits
  while (num != 0)
  {
    int rem = num % 10;
    str[i++] = (rem > 9)? (rem - 10) + 'a' : rem + '0';
    num = num / 10;
  }

  // If number is negative, append '-'
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0'; // Append string terminator

  // Reverse the string
  reverse(str, i);
}

void fill_msg1(char *msg, struct BME280* bme)
{
  msg[0] = 'H';
  msg[1] = 'e';
  msg[2] = 'r';
  msg[3] = 'e';
  msg[4] = ' ';
  msg[5] = 'a';
  msg[6] = 'r';
  msg[7] = 'e';
  msg[8] = ' ';
  msg[9] = 't';
  msg[10] = 'h';
  msg[11] = 'e';
  msg[12] = ' ';
  msg[13] = 'f';
  msg[14] = 'e';
  msg[15] = 't';
  msg[16] = 'c';
  msg[17] = 'h';
  msg[18] = 'e';
  msg[19] = 'd';
  msg[20] = ' ';
  msg[21] = 'd';
  msg[22] = 'a';
  msg[23] = 't';
  msg[24] = 'a';
  msg[25] = ':';
  msg[26] = '\r';
  msg[27] = '\n';

  msg[28] = 't';
  msg[29] = 'e';
  msg[30] = 'm';
  msg[31] = 'p';
  msg[32] = 'e';
  msg[33] = 'r';
  msg[34] = 'a';
  msg[35] = 't';
  msg[36] = 'u';
  msg[37] = 'r';
  msg[38] = 'e';
  msg[39] = ':';
  msg[40] = ' ';

  itoa(msg + 40, bme->temperature);
}

void fill_msg2(char *msg, struct BME280 *bme)
{
  msg[0] = 'p';
  msg[1] = 'r';
  msg[2] = 'e';
  msg[3] = 's';
  msg[4] = 's';
  msg[5] = 'u';
  msg[6] = 'r';
  msg[7] = 'e';
  msg[8] = ':';
  msg[9] = ' ';

  itoa(msg + 10, bme->pressure);
}

void fill_msg3(char *msg, struct BME280 *bme)
{
  msg[0] = 'h';
  msg[1] = 'u';
  msg[2] = 'm';
  msg[3] = 'i';
  msg[4] = 'd';
  msg[5] = 'i';
  msg[6] = 't';
  msg[7] = 'y';
  msg[8] = ':';
  msg[9] = ' ';

  itoa(msg + 10, bme->humidity);
}


uint16_t UART_getchar(void)
{
  while (!USART_GetFlagStatus(USART6, USART_FLAG_RXNE));
  return USART_ReceiveData(USART6);
}
