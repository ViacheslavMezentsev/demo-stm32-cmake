cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PRIVATE Common stlm75 ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    stlm75/stlm75.c
    stm32f1xx_bsp.c
    stm32f1xx_bsp_tsensor.c
)
