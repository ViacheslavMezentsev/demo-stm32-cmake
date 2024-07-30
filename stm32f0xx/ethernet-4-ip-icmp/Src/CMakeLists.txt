cmake_minimum_required(VERSION 3.19)

target_sources(${PROJECT_NAME} PRIVATE
    stm32f0xx_hal_msp.c
    stm32f0xx_it.c
    syscalls.c
    gpio.c
    spi.c
    main.c
)
