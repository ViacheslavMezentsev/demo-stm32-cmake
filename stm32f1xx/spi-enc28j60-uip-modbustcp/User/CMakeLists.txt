cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_sources(${PROJECT_NAME} PRIVATE
    syscalls.c
    bsp_spi1.c
    bsp_usart.c
    enc28j60.c
    main.c
    stm32f10x_it.c
    tapdev.c
    timer.c
)
