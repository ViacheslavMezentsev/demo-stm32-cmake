cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Семейство и модель мк.
set(MCU STM32F103C8T)
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../modules/etl/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(stm32_utils)
include(helpers)

project(${PROJECT_NAME} C CXX ASM)

# Разворачиваем окружение сборки (CMSIS).
set(CMSIS_VERSION "V1.8.5")
set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CMSIS_VERSION}/Drivers")
set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")

# Набор переменных для использования локальной библиотеки Cube, находящейся по указанным путям.
#set(STM32_CMSIS_PATH                "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
#set(STM32_HAL_PATH                  "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
#set(STM32_HAL_INC_PATH              "${STM32_HAL_PATH}/Inc")

add_executable(${PROJECT_NAME} src/startup_stm32f103xb.s src/system_stm32f1xx.c src/syscalls.c src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE c STM32::Nano STM32::Semihosting)

# Опции сборки.
target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F103xB HSE_VALUE=8000000U)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/etl/include
    ${STM32_CMSIS_PATH}/Include
    ${STM32_CMSIS_PATH}/Device/ST/STM32F1xx/Include
)

target_compile_options(${PROJECT_NAME} PRIVATE
    # device type (cortex-m0, cortex-m3, ..)
    -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
    # warning
    -W -Wall -Wextra
    # optimization
    $<$<CONFIG:RELEASE>:-Os>
    -ffunction-sections -fdata-sections
    # debug
    $<$<CONFIG:DEBUG>:-O0 -g3>
    # other
    -pipe
    # c/cxx flags
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive -fno-exceptions -fno-rtti -funsigned-bitfields -fshort-enums -Winline>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/${MCU}x_FLASH.ld
    -mcpu=cortex-m3 -mthumb
    -lc -lgcc
    LINKER:-Map=${PROJECT_NAME}.map,--gc-sections,--no-warn-rwx-segments,--print-memory-usage
)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
