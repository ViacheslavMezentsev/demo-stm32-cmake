cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC Inc)

target_sources(${PROJECT_NAME} PRIVATE
    Src/stm32f4xx_hal_msp.c
    Src/stm32f4xx_it.c
    Src/stm32f4xx_nucleo_bus.c
    Src/stm32f4xx_nucleo.c
    Src/syscalls.c
    Src/sysmem.c
    Src/main.c
)
