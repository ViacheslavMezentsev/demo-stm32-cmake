cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../modules/etl/cmake")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

# Файл конфигурации компоновщика.
set(LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/device/linker/STM32F407VGTx_FLASH.ld)

include(helpers)

project(${PROJECT_NAME} C CXX ASM)

add_executable(${PROJECT_NAME})

# Подключаем необходимые библиотеки.
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/etl/include")

target_link_libraries(${PROJECT_NAME} STM32::Nano STM32::Semihosting)

# Опции сборки.
target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F407xx)

target_compile_options(${PROJECT_NAME} PRIVATE
    # device type (cortex-m0, cortex-m3, ..)
    -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
    # warning
    -W -Wall -Wextra
    # optimization
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-Os>
    -ffunction-sections -fdata-sections
    # debug
    $<$<CONFIG:DEBUG>:-g3>
    #-Wa,-adhlns=${PROJECT_NAME}.lst
    # other
    -pipe
    # c/cxx flags
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive -fno-exceptions -fno-rtti -funsigned-bitfields -fshort-enums -Winline>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${LINKER_FILE}
    -mcpu=cortex-m4 -mthumb
    -lc -lgcc
    LINKER:-Map=${PROJECT_NAME}.map,--no-warn-rwx-segments,--cref,--gc-sections,--print-memory-usage
)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_elf_bin(${PROJECT_NAME})
stm32_generate_elf_hex(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
