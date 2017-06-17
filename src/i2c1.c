#include <i2c1.h>
#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>

void init_I2C1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  I2C_InitTypeDef I2C_InitStruct;

  // enable APB1 peripheral clock for I2C1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  // enable clock for SCL and SDA pins
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Connect I2C1 pins to AF
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // SCL
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA

  // configure I2C1
  I2C_InitStruct.I2C_ClockSpeed = 100000;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_OwnAddress1 = 0x00;

  // disable acknowledge when reading (can be changed later on)
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  // set address length to 7 bit addresses
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &I2C_InitStruct);

  I2C_Cmd(I2C1, ENABLE);
}

void OnTimeout_GetFlagStatus(void)
{
  // Reset I2C
  I2C_DeInit(I2C1);
  init_I2C1();
}

void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
  // wait until I2C1 is not busy anymore
  long timer = BUSY_BUS_TIME;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
  {
    if (--timer <= 0)
    {
      OnTimeout_GetFlagStatus();
      timer = BUSY_BUS_TIME;
    }
  }

  // Send I2C1 START condition
  I2C_GenerateSTART(I2Cx, ENABLE);

  // wait for I2C1 EV5 --> Slave has acknowledged start condition
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  // Send slave Address for write
  I2C_Send7bitAddress(I2Cx, address, direction);

  /* wait for I2C1 EV6, check if
   * either Slave has acknowledged Master transmitter or
   * Master receiver mode, depending on the transmission
   * direction
   */
  if(direction == I2C_Direction_Transmitter)
  {
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  }
  else if(direction == I2C_Direction_Receiver)
  {
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  }
}

void I2C_write(I2C_TypeDef* I2Cx, uint8_t data)
{
  I2C_SendData(I2Cx, data);
  // wait for I2C1 EV8_2 --> byte has been transmitted
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

uint8_t I2C_read_ack(I2C_TypeDef* I2Cx)
{
  // enable acknowledge of recieved data
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  // wait until one byte has been received
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

  // read data from I2C data register and return data byte
  uint8_t data = I2C_ReceiveData(I2Cx);
  return data;
}

uint8_t I2C_read_nack(I2C_TypeDef* I2Cx)
{
  // disable acknowledge of received data
  // nack also generates stop condition after last byte received
  // see reference manual for more info
  I2C_AcknowledgeConfig(I2Cx, DISABLE);
  I2C_GenerateSTOP(I2Cx, ENABLE);

  // wait until one byte has been received
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

  // read data from I2C data register and return data byte
  uint8_t data = I2C_ReceiveData(I2Cx);
  return data;
}

void I2C_stop(I2C_TypeDef* I2Cx){
  // Send I2C1 STOP Condition
  I2C_GenerateSTOP(I2Cx, ENABLE);
}
