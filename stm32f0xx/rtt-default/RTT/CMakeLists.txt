cmake_minimum_required(VERSION 3.19)

add_library(RTT OBJECT)

add_library(RTT::RTT ALIAS RTT)

target_include_directories(RTT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT)

target_sources(RTT PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/RTT/RTT/SEGGER_RTT_printf.c
)
