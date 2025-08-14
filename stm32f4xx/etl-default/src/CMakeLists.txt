cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PUBLIC
    cmsis/include
    device/include
)

target_sources(${PROJECT_NAME} PRIVATE
    cmsis/core/system_stm32f4xx.c
    device/startup/startup_stm32f407xx.s 
    syscalls.c
    main.cpp
)
