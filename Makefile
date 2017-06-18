CROSS_COMPILER ?= arm-none-eabi
CC=${CROSS_COMPILER}-gcc
LD=${CROSS_COMPILER}-ld
OBJCOPY=${CROSS_COMPILER}-objcopy

CFLAGS=	-mcpu=cortex-m4 	\
	-mfloat-abi=hard	\
	-mfpu=fpv4-sp-d16	\
	-mthumb 		\
	-fdata-sections 	\
	-ffunction-sections     \
	-lgcc_eh


LDFLAGS=-gc-sections

CFLAGS += -g3 -I./inc

LD_SCRIPT ?= ld_script.ld

SRCDIR=src
HDRDIR=inc
OBJDIR=obj

SRC=$(addprefix $(SRCDIR)/,      \
    main.c                       \
    stm32f4xx_rcc.c              \
    stm32f4xx_gpio.c             \
    stm32f4xx_exti.c             \
    stm32f4xx_syscfg.c           \
    stm32f4xx_spi.c              \
    stm32f4xx_fmc.c              \
    stm32f4xx_ltdc.c             \
    stm32f4xx_dma2d.c            \
    stm32f4xx_tim.c              \
    stm32f4xx_pwr.c              \
    misc.c                       \
    stm32f4xx_i2c.c              \
    i2c1.c \
    led.c                        \
    timer.c                      \
    button.c                     \
    systick.c                    \
    fonts.c                      \
    bme280.c                     \
    stm32f4xx_usart.c            \
    stm32f429i_discovery.c       \
    stm32f429i_discovery_sdram.c \
    stm32f429i_discovery_lcd.c   \
    stm32f429i_discovery_ioe.c   \
    ltd.c)

OBJ=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o, $(SRC))
BIN=bin

all:
	${CC} ${CFLAGS}  -c ${SRC}
	mkdir -p $(OBJDIR)
	@mv *.o $(OBJDIR)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) $(OBJ)
	cp a.out $(BIN)
	$(OBJCOPY) -O binary $(BIN)

clean:
	$(RM) -rf $(OBJDIR)
	$(RM) -rf $(BIN)
	$(RM) -rf $(OBJ)
	$(RM) -rf a.out
