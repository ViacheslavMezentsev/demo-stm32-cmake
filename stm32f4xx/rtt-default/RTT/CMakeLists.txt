cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT/SEGGER_RTT_printf.c
)
