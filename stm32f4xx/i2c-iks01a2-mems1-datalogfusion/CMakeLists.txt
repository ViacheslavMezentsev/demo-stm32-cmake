cmake_minimum_required(VERSION 3.19)

# Используется набор скриптов для STM32: https://github.com/ObKo/stm32-cmake
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stm32-cmake/cmake/stm32_gcc.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../tools")

include(stm32_utils)
#include(fetch_svd)
#fetch_svd(${MCU})

# 1. ЗАГРУЗКА И ПОДГОТОВКА КОНФИГУРАЦИИ.

# Эта функция использует внешний инструмент 'yq' для преобразования YAML в JSON,
# а затем использует встроенные возможности CMake для разбора JSON.
parse_yaml_config("${CMAKE_CURRENT_SOURCE_DIR}/project_config.yml")
ensure_default_value(ioc_file "") # По умолчанию .ioc файл не используется

# --- ГЛАВНЫЙ БЛОК ОПРЕДЕЛЕНИЯ КОНФИГУРАЦИИ ---
if(ioc_file)
    message(STATUS "Обнаружена настройка 'ioc_file'. Загрузка конфигурации из: ${ioc_file}")
    set(IOC_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ioc_file}")
    
    # Вызываем парсер для .ioc файла
    parse_ioc_file(${IOC_FILE_PATH} "IOC_")

    # Переопределяем ключевые переменные значениями из .ioc
    set(mcu ${IOC_MCU})
    set(project_name ${IOC_PROJECT_NAME})
    set(heap_size ${IOC_HEAP_SIZE})
    set(stack_size ${IOC_STACK_SIZE})
    set(use_hal_components true)

    # Логика для определения версии пакета
    if(IOC_USE_LOCAL_DRIVERS)
        # Если библиотеки скопированы в проект, используем их
        set(cubefw_package "auto")
    else()
        # Иначе используем версию, указанную в .ioc
        set(cubefw_package ${IOC_CUBEFW_PACKAGE})
    endif()

    message(STATUS "Конфигурация из .ioc файла загружена: MCU=${mcu}, Project=${project_name}, Heap=${heap_size}, Stack=${stack_size}")
else()
    message(STATUS "Режим ручной конфигурации (ioc_file не указан).")

    # Если .ioc не используется, устанавливаем значения по умолчанию для YAML-параметров.
    ensure_default_value(project_name "auto")
    ensure_default_value(use_hal_components "true")
    ensure_default_value(heap_size "512")
    ensure_default_value(stack_size "1024")
endif()

# Устанавливаем значения по умолчанию для пропущенных параметров.
ensure_default_value(linker_script "auto")
ensure_default_value(use_newlib_nano "false")

# Определяем языки проекта.
# Если в конфиге не указано, используем C, CXX, ASM по умолчанию.
if(NOT DEFINED languages OR "${languages}" STREQUAL "")
    set(PROJECT_LANGUAGES C CXX ASM)
    message(STATUS "Языки проекта не указаны. Используется по умолчанию: ${PROJECT_LANGUAGES}")
else()
    set(PROJECT_LANGUAGES ${languages})
    message(STATUS "Используются языки проекта из конфига: ${PROJECT_LANGUAGES}")
endif()

# 2. УСТАНОВКА ОСНОВНЫХ ПАРАМЕТРОВ ПРОЕКТА.

set(CMAKE_C_STANDARD ${c_standard})
set(CMAKE_CXX_STANDARD ${cpp_standard})

# Микроконтроллер (можно переопределять через -D).
set(MCU ${mcu} CACHE STRING "Target STM32 microcontroller")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
if(project_name STREQUAL "auto")
    get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
else()
    set(PROJECT_NAME ${project_name})
endif()
message(STATUS "Имя проекта: ${PROJECT_NAME}")

project(${PROJECT_NAME} LANGUAGES ${PROJECT_LANGUAGES})

add_executable(${PROJECT_NAME})

# stm32-cmake автоматически определяет семейство (FAMILY) и тип (TYPE) из имени MCU.
stm32_get_chip_info(${MCU} FAMILY MCU_FAMILY TYPE MCU_TYPE)

# 3. ОПРЕДЕЛЕНИЕ ИСХОДНЫХ ФАЙЛОВ.

# Создаем пустые списки для сбора информации
set(PROJECT_SOURCES "")         # для одиночных файлов

# [НОВОЕ] Конструируем имена файлов, которые мы будем искать
string(TOLOWER ${MCU_FAMILY} MCU_FAMILY_LOWER)
string(TOLOWER ${MCU_TYPE} MCU_TYPE_LOWER)

set(SYSTEM_FILENAME_TARGET "system_stm32${MCU_FAMILY_LOWER}xx.c")

# Шаблон для startup-файла (например, startup_stm32f401xc.s)
set(STARTUP_FILENAME_PATTERN "startup_stm32${MCU_TYPE_LOWER}.*\\.s")

# Перебираем список 'sources' из YAML-конфига.
foreach(src_item IN LISTS sources)
    set(full_path "${CMAKE_CURRENT_SOURCE_DIR}/${src_item}")
    get_filename_component(filename ${src_item} NAME) # Получаем только имя файла
    string(TOLOWER ${filename} filename_lower)

    # --- ПРОВЕРКА НА ОСОБЫЕ СИСТЕМНЫЕ ФАЙЛЫ ---

    # Если файл найден, мы устанавливаем переменную для stm32-cmake
    # и НЕ добавляем его в список для прямой компиляции.    
    if(filename_lower STREQUAL SYSTEM_FILENAME_TARGET)
        set(CMSIS_${MCU_FAMILY}_SYSTEM ${full_path})
        message(STATUS "Обнаружен пользовательский system-файл. Переопределение: ${full_path}")

    elseif(filename_lower MATCHES ${STARTUP_FILENAME_PATTERN})
        set(CMSIS_${MCU_FAMILY}_${MCU_TYPE}_STARTUP ${full_path})
        message(STATUS "Обнаружен пользовательский startup-файл. Переопределение: ${full_path}")
    
    # --- СТАНДАРТНАЯ ОБРАБОТКА ОСТАЛЬНЫХ ФАЙЛОВ ---

    # Вариант 1: Если это директория, подключаем ее как модуль.
    elseif(IS_DIRECTORY ${full_path})
        #message(STATUS "Подключение модуля из директории: ${src_item}")
        add_subdirectory(${src_item})

    # Вариант 2: Если это существующий файл, добавляем его в список исходников.
    elseif(EXISTS ${full_path})
        #message(STATUS "Добавление исходного файла: ${src_item}")
        list(APPEND PROJECT_SOURCES ${full_path})

    # Вариант 3: Если путь не найден.
    else()
        message(WARNING "Источник '${src_item}' не найден и будет проигнорирован.")
    endif()
endforeach()

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

# 4. НАСТРОЙКА ОКРУЖЕНИЯ STM32 И БИБЛИОТЕК.

# Условная обработка HAL.
if(use_hal_components)
    message(STATUS "Автоматическое подключение компонентов HAL/LL включено.")
    ensure_default_value(cubefw_package "auto")

    # Разворачиваем окружение сборки (STM32Cube MCU Firmware Package).
    if(cubefw_package STREQUAL "auto")
        message(STATUS "Режим 'auto': поиск драйверов...")
        
        # ПРИОРИТЕТ 1: Ищем локальные драйверы в папке проекта
        set(LOCAL_DRIVERS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Drivers")

        if(EXISTS "${LOCAL_DRIVERS_PATH}/CMSIS" AND EXISTS "${LOCAL_DRIVERS_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
            message(STATUS "Обнаружены локальные драйверы в '${LOCAL_DRIVERS_PATH}'. Используются они.")
            # Напрямую устанавливаем пути к локальным драйверам
            set(STM32_CMSIS_PATH                "${LOCAL_DRIVERS_PATH}/CMSIS")
            set(STM32_HAL_${MCU_FAMILY}_PATH    "${LOCAL_DRIVERS_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
            set(CUBEFW_PACKAGE "local") # Устанавливаем флаг, что используются локальные драйверы        
            message(STATUS "STM32Cube MCU Firmware Package: ${CUBEFW_PACKAGE}")

        # ПРИОРИТЕТ 2: Если локальные не найдены, ищем последнюю версию в пользовательском репозитории
        else()
            message(STATUS "Локальные драйверы не найдены. Поиск последней версии в пользовательском репозитории...")
            set(CUBE_REPO_PATH "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository")
            
            find_latest_stm32_cube_fw(${MCU_FAMILY} ${CUBE_REPO_PATH} LATEST_FW_VERSION)       
            set(CUBEFW_PACKAGE ${LATEST_FW_VERSION})

            # Устанавливаем пути на основе найденной глобальной версии.
            if(CUBEFW_PACKAGE)
                 message(STATUS "STM32Cube MCU Firmware Package: ${CUBEFW_PACKAGE}")
                 set(STM32_CUBE_${MCU_FAMILY}_PATH   "${CUBE_REPO_PATH}/STM32Cube_FW_${MCU_FAMILY}_${CUBEFW_PACKAGE}/Drivers")
                 set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
                 set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
            endif()
        endif()
    
    # Сценарий, когда версия указана явно.
    else()
        # Используем версию, указанную пользователем.
        set(CUBEFW_PACKAGE ${cubefw_package})
        message(STATUS "Использование указанной версии STM32Cube FW: ${CUBEFW_PACKAGE}")
        
        # Устанавливаем пути на основе указанной глобальной версии.
        set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{CMAKE_USER_HOME}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CUBEFW_PACKAGE}/Drivers")
        set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
        set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")
    endif()
    
    # Финальная проверка и установка пути к CMSIS Device
    if(NOT DEFINED STM32_CMSIS_PATH)
        message(FATAL_ERROR "Не удалось определить пути к драйверам HAL/CMSIS. Проверьте настройки 'cubefw_package' или наличие драйверов.")
    endif()

    set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")

    # Использование локальных файлов.
    #set(CMSIS_${MCU_FAMILY}_SYSTEM                 "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f4xx.c")
    #set(CMSIS_${MCU_FAMILY}_${MCU_TYPE}_STARTUP    "${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f407xx.s")

    find_package(CMSIS COMPONENTS ${MCU} REQUIRED)
    find_package(HAL COMPONENTS STM32${MCU_FAMILY} REQUIRED)

    # Автоматически добавляем USE_HAL_DRIVER, если в конфиге указан хотя бы один HAL-компонент.
    # Команда if() для списка считается истинной, если список не пуст.
    if(hal_components)
        #message(STATUS "Обнаружены компоненты HAL/LL. Автоматически добавляется определение: USE_HAL_DRIVER")
        # Подключаем необходимые библиотеки на основе конфига.
        # Сначала создаем список целевых HAL-библиотек, добавляя префикс к каждому элементу.
        list(TRANSFORM hal_components PREPEND "HAL::STM32::${MCU_FAMILY}::" OUTPUT_VARIABLE HAL_TARGETS)

        target_compile_definitions(${PROJECT_NAME} PRIVATE USE_HAL_DRIVER)

        # CMSIS и некоторые дополнительные компоненты. 
        target_link_libraries(${PROJECT_NAME} PRIVATE CMSIS::STM32::${MCU_TYPE})

        # Проверяем, есть ли среди компонентов LL-драйверы.
        set(LL_DRIVER_FOUND FALSE)
        foreach(component IN LISTS hal_components)
            if(component MATCHES "^LL_")
                set(LL_DRIVER_FOUND TRUE)
                break()
            endif()
        endforeach()
        
        if(LL_DRIVER_FOUND)
            #message(STATUS "Обнаружены LL компоненты. Автоматически добавляется определение: USE_FULL_LL_DRIVER")
            target_compile_definitions(${PROJECT_NAME} PRIVATE USE_FULL_LL_DRIVER)
        endif()
    endif()
else()
    message(STATUS "Автоматическое подключение компонентов HAL/LL отключено. Убедитесь, что все необходимые исходники указаны в секции 'sources'.")
    set(HAL_TARGETS "") # Убеждаемся, что список целей HAL пуст
endif()

# Теперь подключаем все библиотеки одним вызовом.
target_link_libraries(${PROJECT_NAME} PRIVATE    
    ${HAL_TARGETS}              # Сформированный список HAL-компонентов
    ${link_libraries}           # Дополнительные библиотеки
)

# Добавляем системные библиотеки C/C++.
# Использование newlib-nano для уменьшения размера кода.
if(use_newlib_nano)
    target_link_libraries(${PROJECT_NAME} PRIVATE STM32::Nano)
endif()

# Заглушки для системных вызовов. Выберите ИЛИ NoSys, ИЛИ Semihosting.
if(system_library STREQUAL "NoSys")
    target_link_libraries(${PROJECT_NAME} PRIVATE STM32::NoSys)
# Альтернатива для отладки через SWD/JTAG.
elseif(system_library STREQUAL "Semihosting")
    target_link_libraries(${PROJECT_NAME} PRIVATE STM32::Semihosting)
endif()

# 5. НАСТРОЙКИ КОМПИЛЯЦИИ И КОМПОНОВКИ.

# Добавляем пути к заголовочным файлам из конфига.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${include_directories}
)

# Добавляем определения компилятора из конфига + обязательные.
# Примечание: STM32${MCU_TYPE} генерируется CMake, его лучше оставить здесь для надежности.
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    ${compile_definitions}
    STM32${MCU_TYPE}
)

target_compile_options(${PROJECT_NAME} PRIVATE 
#    $<$<CONFIG:DEBUG>:-Og -gdwarf-2 -gstrict-dwarf>    # для симуляции в Proteus
    $<$<CONFIG:DEBUG>:-O0 -g3>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive -fno-exceptions -fno-rtti -fno-unwind-tables -fno-threadsafe-statics -fno-use-cxa-atexit -fstrict-enums>
    # Пользовательские опции
    ${compile_options}
)

# Добавляем опции и директивы компоновщика из конфига.
if("map" IN_LIST build_artifacts)
    target_link_options(${PROJECT_NAME} PRIVATE LINKER:-Map=${PROJECT_NAME}.map)
endif()

# Опции, передаваемые компилятору (например, -lm)
target_link_options(${PROJECT_NAME} PRIVATE ${link_options})

# Прямые директивы для компоновщика (например, --print-memory-usage)
foreach(directive IN LISTS linker_directives)
    target_link_options(${PROJECT_NAME} PRIVATE "LINKER:${directive}")
endforeach()

# 6. ГЕНЕРАЦИЯ ФАЙЛОВ И АРТЕФАКТОВ СБОРКИ.

# Управление скриптом компоновщика.
if(linker_script STREQUAL "auto")
    # Размеры кучи и стека (можно переопределять через -D).
    # Нормализуем значения размеров памяти в байты.
    normalize_memory(heap_size)
    normalize_memory(stack_size)

    set(HEAP_SIZE ${heap_size} CACHE STRING "Required amount of heap")
    set(STACK_SIZE ${stack_size} CACHE STRING "Required amount of stack")

    # Проверяем версию GCC и задаём USE_READONLY.
    if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
        set(USE_READONLY "(READONLY)")
        message(STATUS "Using READONLY in linker script (GCC >= 11.0)")
    else()
        set(USE_READONLY "")
        message(STATUS "Not using READONLY in linker script (GCC < 11.0)")
    endif()

    # Генерируем .ld-файл из шаблона.
    message(STATUS "Генерация скрипта компоновщика из шаблона...")
    string(REGEX REPLACE "..$" "XX" MCU_TYPE_GENERIC "${MCU_TYPE}")
    configure_file("${CMAKE_SOURCE_DIR}/STM32${MCU_TYPE_GENERIC}_FLASH.ld.in" "${CMAKE_BINARY_DIR}/STM32${MCU_TYPE}_FLASH.ld" @ONLY)
    stm32_add_linker_script(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/STM32${MCU_TYPE}_FLASH.ld")
else()
    # Подключаем конфигурационный файл компоновщика.
    set(custom_script_path "${CMAKE_CURRENT_SOURCE_DIR}/${linker_script}")
    if(EXISTS ${custom_script_path})
        message(STATUS "Использование пользовательского скрипта компоновщика: ${custom_script_path}")
        stm32_add_linker_script(${PROJECT_NAME} PRIVATE ${custom_script_path})
    else()
        message(FATAL_ERROR "Указанный скрипт компоновщика не найден: ${custom_script_path}")
    endif()
endif()

# Генерируем артефакты сборки на основе списка из конфигурации.
stm32_print_size_of_target(${PROJECT_NAME})

if("bin" IN_LIST build_artifacts)
    stm32_generate_binary_file(${PROJECT_NAME})
endif()
if("hex" IN_LIST build_artifacts)
    stm32_generate_hex_file(${PROJECT_NAME})
endif()
if("lss" IN_LIST build_artifacts)
    stm32_generate_lss_file(${PROJECT_NAME})
endif()
