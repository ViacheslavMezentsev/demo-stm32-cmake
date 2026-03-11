cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC Inc)

target_sources(${PROJECT_NAME} PRIVATE
    Src/stm32h5xx_hal_msp.c
    Src/stm32h5xx_it.c
    #Src/syscalls.c
    Src/sysmem.c
    Src/system_stm32h5xx.c
    Src/gpio.c
    Src/icache.c
    Src/crc.c
    Src/main.c
)
