cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PRIVATE Inc )

target_sources(${PROJECT_NAME} PRIVATE
    Src/stm32f0xx_hal_msp.c
    Src/stm32f0xx_it.c
    Src/dma.c
    Src/usart.c
    Src/gpio.c
    Src/main.c
)
