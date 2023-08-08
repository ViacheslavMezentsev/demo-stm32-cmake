cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_sources(${PROJECT_NAME} PRIVATE core/system_stm32f4xx.c)
