cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

project(${PROJECT_NAME} C CXX)

add_library(project_options INTERFACE)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(modm)

target_link_libraries(${PROJECT_NAME} modm_options modm_warnings modm)

# Артефакты сборки.
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_elf_bin(${PROJECT_NAME})
stm32_generate_elf_hex(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
