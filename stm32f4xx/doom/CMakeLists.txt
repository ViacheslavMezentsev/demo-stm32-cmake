cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

# Файл конфигурации компоновщика.
set(LINKER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/device/linker/STM32F429ZITx_FLASH.ld")

project(${PROJECT_NAME} C)

add_executable(${PROJECT_NAME})

# Подключаем необходимые библиотеки.
add_subdirectory(cmsis)
add_subdirectory(device)
add_subdirectory(driver)
add_subdirectory(fatfs)
add_subdirectory(usbhost)
add_subdirectory(chocdoom)
add_subdirectory(user)

target_link_libraries(${PROJECT_NAME} m)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Опции сборки.
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:DEBUG_> STM32F4XX USE_STDPERIPH_DRIVER STM32F429_439xx)

target_compile_options(${PROJECT_NAME} PRIVATE
    # device type (cortex-m0, cortex-m3, ..)
    -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -fcommon
    -fdata-sections -ffunction-sections
    # warning
    -Wall
    # optimization
    $<$<CONFIG:DEBUG>:-O2>
    $<$<CONFIG:RELEASE>:-Os>
    # debug
    #-Wa,-adhlns=${PROJECT_NAME}.lst
    # other
    -c
)

target_link_options(${PROJECT_NAME} PRIVATE
    -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian
    -nostartfiles
    -T${LINKER_FILE} LINKER:-Map=${PROJECT_NAME}.map,--gc-sections,--print-memory-usage
)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
