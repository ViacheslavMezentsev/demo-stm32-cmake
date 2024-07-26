cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

# Семейство и модель мк.
set(MCU STM32H743XIH6)
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake/cmake/stm32_gcc.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(stm32_utils)
#include(fetch_svd)
#fetch_svd(${MCU})

project(${PROJECT_NAME} C CXX ASM)

# Разворачиваем окружение сборки (HAL, CMSIS, and CMSIS-SVD).
set(CMSIS_VERSION "V1.11.2")
set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CMSIS_VERSION}/Drivers")
set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")

# Набор переменных для использования локальной библиотеки Cube, находящейся по указанным путям.
#set(STM32_CMSIS_PATH                "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
#set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")
#set(STM32_HAL_${MCU_FAMILY}_PATH    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32${MCU_FAMILY}xx_HAL_Driver")

#set(CMSIS_H7_M7_SYSTEM              "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32h7xx.c")
#set(CMSIS_H7_M7_H743xx_STARTUP      "${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32h743xihx.s")

stm32_get_chip_type(${MCU_FAMILY} ${MCU_MODEL} MCU_TYPE)
stm32_fetch_cmsis(${MCU_FAMILY})
stm32_fetch_hal(${MCU_FAMILY})

find_package(CMSIS COMPONENTS STM32${MCU_FAMILY}_M7 REQUIRED)
find_package(HAL COMPONENTS STM32${MCU_FAMILY}_M7 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

# Подключаем необходимые библиотеки.
target_link_libraries(${PROJECT_NAME}
    HAL::STM32::${MCU_FAMILY}::M7::RCC
    HAL::STM32::${MCU_FAMILY}::M7::GPIO
    HAL::STM32::${MCU_FAMILY}::M7::UART
    HAL::STM32::${MCU_FAMILY}::M7::CORTEX
    CMSIS::STM32::${MCU_MODEL}::M7
    STM32::Nano
    STM32::NoSys
    STM32::Semihosting
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_HAL_DRIVER STM32H743xx)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-Os> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti -fno-exceptions>)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-warn-rwx-segments,--print-memory-usage)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
