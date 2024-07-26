cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 99)

# Семейство и модель мк.
set(MCU STM32F103C6T)
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(stm32_utils)

project(${PROJECT_NAME} C)

# Разворачиваем окружение сборки (CMSIS).
set(CMSIS_VERSION "V1.8.5")
set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CMSIS_VERSION}/Drivers")
set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")

# Набор переменных для использования локальной библиотеки Cube, находящейся по указанным путям.
#set(STM32_CMSIS_PATH                "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")

add_executable(${PROJECT_NAME} syscalls.c main.c)

# Подключаем необходимые библиотеки.
target_link_libraries(${PROJECT_NAME} STM32::Nano STM32::NoSys)

target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F103x6)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STM32_CMSIS_PATH}/Include
    ${STM32_CMSIS_PATH}/Device/ST/STM32F1xx/Include)

target_compile_options(${PROJECT_NAME} PRIVATE
    # device type (cortex-m0, cortex-m3, ..)
    -mcpu=cortex-m0 -mthumb
    # warning
    -W -Wall -Wextra
    # optimization
    $<$<CONFIG:DEBUG>:-Os -g -gdwarf-2 -gstrict-dwarf>
    $<$<CONFIG:RELEASE>:-Os>
    -ffunction-sections -fdata-sections
    -fsingle-precision-constant
    # other
)

target_link_options(${PROJECT_NAME} PRIVATE
    -nostdlib -nostartfiles
    -T${CMAKE_CURRENT_SOURCE_DIR}/${MCU}x_FLASH.ld
    LINKER:--gc-sections,--no-warn-rwx-segments,--print-memory-usage
)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
