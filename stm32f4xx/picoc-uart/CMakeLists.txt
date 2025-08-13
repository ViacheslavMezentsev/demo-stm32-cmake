cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

# Модель мк.
# Микроконтроллер можно переопределить из командной строки: -DMCU=STM32F103C8
set(MCU "STM32F411CEU" CACHE STRING "Target STM32 microcontroller")
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

# Размеры кучи и стека (можно переопределять через -D).
set(HEAP_SIZE  "0K" CACHE STRING "Required amount of heap")
set(STACK_SIZE "8K" CACHE STRING "Required amount of stack")

# Используется набор скриптов для STM32: https://github.com/ObKo/stm32-cmake
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake/cmake/stm32_gcc.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(stm32_utils)
#include(fetch_svd)
#fetch_svd(${MCU})

project(${PROJECT_NAME} C CXX ASM)

# stm32-cmake автоматически определяет семейство (FAMILY) и тип (TYPE) из имени MCU.
stm32_get_chip_info(${MCU} FAMILY MCU_FAMILY TYPE MCU_TYPE)

# Разворачиваем окружение сборки (HAL, CMSIS и CMSIS-SVD).
set(CMSIS_VERSION "V1.28.2")
set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CMSIS_VERSION}/Drivers")
set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")

# Набор переменных для использования локальной библиотеки Cube, находящейся по указанным путям.
#set(STM32_CMSIS_PATH                "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
#set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")
#set(STM32_HAL_${MCU_FAMILY}_PATH    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32${MCU_FAMILY}xx_HAL_Driver")

# Использование локальных файлов.
set(CMSIS_${MCU_FAMILY}_SYSTEM                  "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f4xx.c")
set(CMSIS_${MCU_FAMILY}_${MCU_TYPE}_STARTUP     "${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f411xe.s")

find_package(CMSIS COMPONENTS ${MCU} REQUIRED)
find_package(HAL COMPONENTS STM32${MCU_FAMILY} REQUIRED)

add_executable(${PROJECT_NAME})

# Подключаем необходимые библиотеки.
add_subdirectory(Core)
add_subdirectory(picoc)

target_link_libraries(${PROJECT_NAME} PRIVATE
    HAL::STM32::${MCU_FAMILY}::RCCEx
    HAL::STM32::${MCU_FAMILY}::DMA
    HAL::STM32::${MCU_FAMILY}::GPIO
    HAL::STM32::${MCU_FAMILY}::UART
    HAL::STM32::${MCU_FAMILY}::CORTEX
    
    # CMSIS Core and device specific files
    # Выберите ИЛИ ${MCU_MODEL}, ИЛИ ${MCU_TYPE}.
    #CMSIS::STM32::${MCU_MODEL} # для .ld-файла по умолчанию
    CMSIS::STM32::${MCU_TYPE}

    # C/C++ Standard Libraries
    STM32::Nano              # Использование newlib-nano для уменьшения размера кода.
    STM32::NoSys             # Заглушки для системных вызовов. Выберите ИЛИ NoSys, ИЛИ Semihosting.
    #STM32::Semihosting       # Альтернатива для отладки через SWD/JTAG.
)

target_include_directories(${PROJECT_NAME} PRIVATE    
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc    # stm32f4xx_hal_conf.h
)

target_compile_definitions(${PROJECT_NAME} PRIVATE EMBEDDED_HOST USE_HAL_DRIVER STM32${MCU_TYPE})
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-Os> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti -fno-exceptions>)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:--print-memory-usage)

# Проверяем версию GCC и задаём USE_READONLY.
if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
    set(USE_READONLY "(READONLY)")
    message(STATUS "Using READONLY in linker script (GCC >= 11.0)")
else()
    set(USE_READONLY "")
    message(STATUS "Not using READONLY in linker script (GCC < 11.0)")
endif()

# Генерируем .ld-файл из шаблона.
configure_file("${CMAKE_SOURCE_DIR}/STM32F411XX_FLASH.ld.in" "${CMAKE_BINARY_DIR}/STM32${MCU_TYPE}_FLASH.ld" @ONLY)

# Подключаем конфигурационный файл компоновщика.
stm32_add_linker_script(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/STM32${MCU_TYPE}_FLASH.ld")

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
