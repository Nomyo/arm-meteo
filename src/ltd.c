#include <stm32f429i_discovery.h>
#include <stm32f429i_discovery_lcd.h>
#include <stm32f4xx.h>
#include <logo.h>

void LCD_Config(void);
void LCD_AF_GPIOConfig(void);
void delay(__IO uint32_t nCount);


/**
 * @brief LCD configuration.
 * @note  This function Configure tha LTDC peripheral :
 *        1) Configure the Pixel Clock for the LCD
 *        2) Configure the LTDC Timing and Polarity
 *        3) Configure the LTDC Layer 1 :
 *           - The frame buffer is located at SDRAM buffer
 *           - The Layer size configuration : 240x320
 *        4) Configure the Color Keying for Layer 1.
 * @retval
 *  None
 */

void LCD_Config(void)
{
  LTDC_InitTypeDef               LTDC_InitStruct;
  LTDC_Layer_InitTypeDef         LTDC_Layer_InitStruct;
  GPIO_InitTypeDef               GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure NCS in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure the LCD Control pins ------------------------------------------*/
  LCD_CtrlLinesConfig();
  LCD_ChipSelect(DISABLE);
  LCD_ChipSelect(ENABLE);

  /* Configure the LCD_SPI interface -----------------------------------------*/
  LCD_SPIConfig();

  /* Power on the LCD --------------------------------------------------------*/
  LCD_PowerOn();

  /* Enable the LTDC Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

  /* Configure the LCD Control pins */
  LCD_AF_GPIOConfig();

  /* Configure the FMC Parallel interface : SDRAM is used as Frame Buffer for LCD */
  SDRAM_Init();

/* Enable Pixel Clock --------------------------------------------------------*/

  /* Configure PLLSAI prescalers for LCD */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/3 = 64 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64/8 = 8 Mhz */
  RCC_PLLSAIConfig(192, 7, 3);
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);

  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  /* Wait for PLLSAI activation */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }

/* LTDC Initialization -------------------------------------------------------*/

  /* Initialize the horizontal synchronization polarity as active low*/
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
  /* Initialize the vertical synchronization polarity as active low */
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
  /* Initialize the data enable polarity as active low */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
  /* Initialize the pixel clock polarity as input pixel clock */
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

  /* Timing configuration */
  /* Configure horizontal synchronization width */
  LTDC_InitStruct.LTDC_HorizontalSync = 9;
  /* Configure vertical synchronization height */
  LTDC_InitStruct.LTDC_VerticalSync = 1;
  /* Configure accumulated horizontal back porch */
  LTDC_InitStruct.LTDC_AccumulatedHBP = 29;
  /* Configure accumulated vertical back porch */
  LTDC_InitStruct.LTDC_AccumulatedVBP = 3;
  /* Configure accumulated active width */
  LTDC_InitStruct.LTDC_AccumulatedActiveW = 269;
  /* Configure accumulated active height */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = 323;
  /* Configure total width */
  LTDC_InitStruct.LTDC_TotalWidth = 279;
  /* Configure total height */
  LTDC_InitStruct.LTDC_TotalHeigh = 327;

  LTDC_Init(&LTDC_InitStruct);

  /* Configure R,G,B component values for LCD background color */
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;

  LTDC_Init(&LTDC_InitStruct);

/* LTDC initialization end ---------------------------------------------------*/

/* Layer1 Configuration ------------------------------------------------------*/

  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 30
     Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
     Vertical start   = vertical synchronization + vertical back porch     = 4
     Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (240 + 30 - 1);
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (160 + 4 - 1); //320

  /* Pixel Format configuration*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;

  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;

  /* Configure blending factors */
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

  /* Default Color configuration (configure A,R,G,B component values) */
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 255;
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 255;
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 255;
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 255;

  /* Input Address configuration */
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)&logo;

  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3
     Active high width         = 240
     number of bytes per pixel = 2    (pixel_format : RGB565)
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((60 * 2) + 3);

  /*  the pitch is the increment from the start of one line of pixels to the
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel
  */
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (60 * 2);

  /* configure the number of lines */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 60;

  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  LTDC_DitherCmd(ENABLE);
}


void LCD_AF_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOI, GPIOJ, GPIOG, GPIOF, GPIOH AHB Clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
                         RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

/* GPIOs Configuration */
/*
  +------------------------+-----------------------+----------------------------+
  +                       LCD pins assignment                                   +
  +------------------------+-----------------------+----------------------------+
  |  LCD_TFT R2 <-> PC.12  |  LCD_TFT G2 <-> PA.06 |  LCD_TFT B2 <-> PD.06      |
  |  LCD_TFT R3 <-> PB.00  |  LCD_TFT G3 <-> PG.10 |  LCD_TFT B3 <-> PG.11      |
  |  LCD_TFT R4 <-> PA.11  |  LCD_TFT G4 <-> PB.10 |  LCD_TFT B4 <-> PG.12      |
  |  LCD_TFT R5 <-> PA.12  |  LCD_TFT G5 <-> PB.11 |  LCD_TFT B5 <-> PA.03      |
  |  LCD_TFT R6 <-> PB.01  |  LCD_TFT G6 <-> PC.07 |  LCD_TFT B6 <-> PB.08      |
  |  LCD_TFT R7 <-> PG.06  |  LCD_TFT G7 <-> PD.03 |  LCD_TFT B7 <-> PB.09      |
  -------------------------------------------------------------------------------
  |  LCD_TFT HSYNC <-> PC.06  | LCDTFT VSYNC <->  PA.04 |
  |  LCD_TFT CLK   <-> PG.07  | LCD_TFT DE   <->  PF.10 |
  -----------------------------------------------------

*/

  /* GPIOA configuration */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | \
    GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* GPIOB configuration */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | \
    GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;

  GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* GPIOC configuration */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_LTDC);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;// | GPIO_Pin_10;

  GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;

  GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

  GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | \
    GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/**
 * @brief  Inserts a delay time.
 * @param  nCount: specifies the delay time length.
 * @retval None
 */
void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  //for(index = 100000*nCount; index != 0; index--)
  for(index = 100*nCount; index != 0; index--)
  {
  }
}
