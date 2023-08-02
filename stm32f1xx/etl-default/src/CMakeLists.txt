cmake_minimum_required(VERSION 3.16)

# List of include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    cmsis/include
    device/include
)

target_sources(${PROJECT_NAME} PRIVATE
    device/startup/startup_stm32f103xb.s
    cmsis/core/system_stm32f1xx.c
#    syscalls.c
    main.cpp
)

# Подключение библиотеки semihosting'а.
#target_link_options(${PROJECT_NAME} PRIVATE -l rdimon $<$<C_COMPILER_ID:GNU>:-specs=rdimon.specs>)
