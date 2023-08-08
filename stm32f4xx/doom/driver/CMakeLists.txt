cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_sources(${PROJECT_NAME} PRIVATE 
    src/stm32f4xx_dma.c
    src/stm32f4xx_dma2d.c
    src/stm32f4xx_exti.c
    src/stm32f4xx_fmc.c
    src/stm32f4xx_gpio.c
    src/stm32f4xx_i2c.c
    src/stm32f4xx_ltdc.c
    src/stm32f4xx_rcc.c
    src/stm32f4xx_sdio.c
    src/stm32f4xx_spi.c
    src/stm32f4xx_syscfg.c
    src/stm32f4xx_tim.c
    src/stm32f4xx_usart.c
    src/misc.c
)
