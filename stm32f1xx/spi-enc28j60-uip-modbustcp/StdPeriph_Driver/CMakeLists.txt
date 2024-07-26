cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC inc )

target_sources(${PROJECT_NAME} PRIVATE
    src/misc.c
    #src/stm32f10x_adc.c
    #src/stm32f10x_bkp.c
    #src/stm32f10x_can.c
    #src/stm32f10x_cec.c
    #src/stm32f10x_crc.c
    #src/stm32f10x_dac.c
    #src/stm32f10x_dbgmcu.c
    #src/stm32f10x_dma.c
    #src/stm32f10x_exti.c
    #src/stm32f10x_flash.c
    #src/stm32f10x_fsmc.c
    src/stm32f10x_gpio.c
    #src/stm32f10x_i2c.c
    #src/stm32f10x_iwdg.c
    #src/stm32f10x_pwr.c
    src/stm32f10x_rcc.c
    #src/stm32f10x_rtc.c
    #src/stm32f10x_sdio.c
    src/stm32f10x_spi.c
    #src/stm32f10x_tim.c
    src/stm32f10x_usart.c
    #src/stm32f10x_wwdg.c
    #src/system_stm32f10x.c
)
