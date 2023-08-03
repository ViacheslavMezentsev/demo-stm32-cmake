cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

# Семейство и модель мк.
set(MCU STM32F103C8T)
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake/cmake/stm32_gcc.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(fetch_svd)

project(${PROJECT_NAME} C CXX ASM)

# Разворачиваем окружение сборки (HAL, CMSIS, and CMSIS-SVD).
set(CMSIS_VERSION "V1.8.5")
set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{USERPROFILE}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CMSIS_VERSION}/Drivers")
set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")
set(CMSIS_${MCU_FAMILY}_SYSTEM                 "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f1xx.c")
set(CMSIS_${MCU_FAMILY}_F103xB_STARTUP         "${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f103xb.s")

#fetch_svd(${MCU})
stm32_get_chip_type(${MCU_FAMILY} ${MCU_MODEL} MCU_TYPE)
stm32_fetch_cmsis(${MCU_FAMILY})
stm32_fetch_hal(${MCU_FAMILY})

find_package(CMSIS COMPONENTS ${MCU} REQUIRED)
find_package(HAL COMPONENTS STM32${MCU_FAMILY} REQUIRED)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем необходимые библиотеки.
add_subdirectory(Core)
add_subdirectory(pcf8574-afiskon)
add_subdirectory(User)

target_link_libraries(${PROJECT_NAME}    
    HAL::STM32::${MCU_FAMILY}::RCC
    HAL::STM32::${MCU_FAMILY}::DMA
    HAL::STM32::${MCU_FAMILY}::GPIO
    HAL::STM32::${MCU_FAMILY}::I2C
    HAL::STM32::${MCU_FAMILY}::CORTEX
    CMSIS::STM32::${MCU_TYPE}
    STM32::Nano
    STM32::NoSys
)

target_compile_options(${PROJECT_NAME} PRIVATE -Os $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti -fno-exceptions>)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-warn-rwx-segments,--print-memory-usage)

# Подключаем конфигурационный файл компоновщика.
stm32_add_linker_script(${PROJECT_NAME} PRIVATE ${MCU}x_FLASH.ld)

stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_elf_bin(${PROJECT_NAME})
stm32_generate_elf_hex(${PROJECT_NAME})
