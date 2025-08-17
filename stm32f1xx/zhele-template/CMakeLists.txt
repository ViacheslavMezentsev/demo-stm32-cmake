cmake_minimum_required(VERSION 3.19)

# Используется набор скриптов для STM32: https://github.com/ObKo/stm32-cmake
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake/cmake/stm32_gcc.cmake")

# Определяем путь к фреймворку stm32-cmake-yml.
set(STM32_YML_FRAMEWORK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake-yml")

# Добавляем путь к фреймворку в CMAKE_MODULE_PATH.
list(APPEND CMAKE_MODULE_PATH "${STM32_YML_FRAMEWORK_DIR}")

# Подключаем главный файл фреймворка.
include(stm32_yml)

# (Опционально) Если вы хотите использовать нестандартное имя для конфига.
#set(PROJECT_CONFIG_FILE "config.yml" CACHE STRING "..." FORCE)

# Вызываем подготовительную функцию.
# Она прочитает YAML и вернет нам имя проекта и список языков.
stm32_yml_prepare_project_data(PROJECT_NAME PROJECT_LANGUAGES)

project(${PROJECT_NAME} LANGUAGES ${PROJECT_LANGUAGES})

# Вызываем основную функцию настройки.
stm32_yml_setup_project(${PROJECT_NAME})
