# Коллекция демо-проектов STM32: VS Code + CMake + YAML

Этот репозиторий содержит коллекцию примеров проектов для различных семейств микроконтроллеров **STM32** (F0, F1, F4, F7, H5, H7).

Все проекты переведены на использование современного фреймворка сборки **[stm32-cmake-yml](https://github.com/ViacheslavMezentsev/stm32-cmake-yml)**. Это позволяет отказаться от сложного редактирования `CMakeLists.txt` в пользу простого и декларативного файла конфигурации **`project_config.yml`**.

🎥 **[Смотреть видео-инструкцию: Конфигурирование сборки проектов STM32 при помощи скрипта YAML](https://www.youtube.com/watch?v=mdJv4qNZnEY)**

---

## 📂 Структура репозитория

Репозиторий организован по семействам микроконтроллеров. Внутри каждой папки находятся готовые шаблоны проектов:

* `stm32f0xx/` — проекты для Cortex-M0
* `stm32f1xx/` — проекты для Cortex-M3 (например, Blue Pill)
* `stm32f4xx/` — проекты для Cortex-M4 (например, Black Pill, Nucleo)
* `stm32f7xx/` — проекты для Cortex-M7
* `stm32h5xx/` — проекты для Cortex-M33
* `stm32h7xx/` — проекты для двухъядерных Cortex-M7/M4

Каждая папка проекта является самодостаточной (содержит свои исходники и конфиги), но использует общие настройки VS Code и модули сборки.

## 🛠 Требования и установка

Для работы с проектами вам потребуется настроенное окружение.

### Основные инструменты

1. **Visual Studio Code** с расширениями:
    * *C/C++*
    * *CMake Tools*
    * *Cortex-Debug*
2. **xPack GNU Arm Embedded GCC** (компилятор)
3. **CMake** (система сборки)
4. **Ninja** (генератор сборки, работает быстрее Make)
5. **yq** (утилита для парсинга YAML, **обязательно**)
6. **OpenOCD / драйверы ST-Link/J-Link** (для отладки)

### 🚀 Быстрая установка

#### Windows (через Scoop)

Рекомендуемый способ. Запустите PowerShell от имени пользователя:

```powershell
# Установка Scoop (если нет)
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser; irm get.scoop.sh | iex

# Установка инструментов
scoop install cmake ninja openocd yq

# Установка компилятора (вручную скачайте xPack GCC и добавьте в PATH, либо через scoop если есть бакет)
```

*Примечание: Убедитесь, что путь к `arm-none-eabi-gcc` добавлен в системную переменную `PATH`.*

#### Linux (Ubuntu / Debian / WSL2)

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build openocd
sudo snap install yq

# Скачайте и распакуйте xPack GCC, добавьте путь к bin в ~/.bashrc
```

#### Windows + WSL2 (Рекомендуется)

Если вы используете WSL2, не забудьте настроить проброс USB-отладчика через `usbipd-win`, чтобы иметь возможность прошивать плату из Linux-окружения.

---

## 📥 Начало работы

Клонируйте репозиторий с флагом `--recurse-submodules`, чтобы подтянуть сам фреймворк сборки и зависимости:

```bash
git clone --recurse-submodules https://github.com/ViacheslavMezentsev/demo-stm32-cmake.git
cd demo-stm32-cmake
```

## ⚙️ Как настроить и собрать проект

Вам больше не нужно править сложные CMake-файлы. Вся настройка производится в файле **`project_config.yml`** в корне папки конкретного проекта.

### Пример конфигурации (`stm32_config.yml`)

Вот как выглядит типичный конфиг (на примере `stm32f1xx/cmsis-01-default`):

```yaml
# Версия фреймворка
stm32_cmake_yml_version: "0.4"

# --- Параметры проекта ---
# Если используете CubeMX, укажите файл .ioc, и параметры MCU подтянутся автоматически
# ioc_file: "my_project.ioc"

# Для ручной настройки:
mcu: "STM32F103C8T6"   # Точное имя чипа
heap_size: "512"       # Размер кучи
stack_size: "1K"       # Размер стека

# --- Исходные файлы ---
sources:
  - "xprintf"          # Папка с исходниками (должна содержать CMakeLists.txt)
  - "syscalls.c"       # Отдельный файл
  - "uart.cpp"
  - "main.cpp"

# --- Драйверы ---
use_cmsis: true        # Подключить CMSIS (startup, system файлы)
use_hal: true          # Подключить HAL
use_freertos: false    # Подключить FreeRTOS

# Какие модули HAL нужны
hal_components: [ "RCC", "GPIO", "UART", "CORTEX" ]

# --- Компиляция ---
c_standard: 17
cpp_standard: 17
compile_options:
  - "-Os"              # Оптимизация по размеру
  - "-g3"              # Отладочная информация

# --- Системные настройки ---
use_newlib_nano: true      # Использовать легкую библиотеку
system_library: "NoSys"    # Заглушки системных вызовов
build_artifacts: [ "bin", "hex", "map", "lss" ]

# --- Отладка сборки ---
verbose_build: false       # Показывать полные команды компиляции в логе
validate_linker_script: true # Проверять, влезает ли стек/куча в RAM
```

### Процесс сборки в VS Code

1. Откройте папку репозитория (или конкретного проекта) в VS Code.
2. Выберите кит компилятора (`arm-none-eabi-gcc`), если CMake Tools спросит.
3. Нажмите **F7** (или кнопку "Build" в нижней панели).

**Что происходит в консоли?**
CMake прочитает ваш YAML и сообщит о своих действиях. Пример вывода:

```text
[cmake] -- Конфигурация из .../stm32_config.yml успешно загружена.
[cmake] -- Определено имя проекта: cmsis-01-default
[cmake] -- Режим 'auto': поиск драйверов...
[cmake] -- Обнаружены локальные драйверы в '.../Drivers'. Используются они.
[cmake] -- Автоматическое подключение CMSIS включено.
[cmake] -- Автоматическое подключение компонентов HAL/LL отключено (так как use_hal: false).
[cmake] -- Генерация скрипта компоновщика из шаблона...
[cmake] -- Проверка размера RAM в скрипте компоновщика пройдена успешно. (20K)
```

*Обратите внимание: система сама находит драйверы, генерирует ld-скрипт и проверяет корректность параметров памяти.*

---

## 📝 Как создать новый проект

Чтобы создать свой проект для любой линейки (F0-H7), выполните следующие шаги:

1. **Создайте папку** внутри соответствующей директории семейства (например, `stm32h7xx/my-new-project`).
2. **Скопируйте файлы** из любого соседнего примера (например, из `01-default`):
    * `CMakeLists.txt` (он стандартный и ссылается на общий фреймворк).
    * `stm32_config.yml` (шаблон конфигурации).
    * Папку `.vscode` (если хотите локальные настройки, иначе используются глобальные из корня).
3. **Добавьте исходный код:**
    * **Вариант А (С чистого листа):** Создайте `main.c` и пропишите его в `sources` в YAML.
    * **Вариант Б (CubeMX):** Сгенерируйте проект в CubeMX (выберите Toolchain/IDE: **Makefile**). Скопируйте `.ioc` файл и сгенерированные папки (`Core`, `Drivers`) в папку проекта.
4. **Настройте `stm32_config.yml`:**
    * Если есть `.ioc` файл, раскомментируйте `ioc_file: "my.ioc"`. Скрипт сам возьмет оттуда модель MCU.
    * Если нет, пропишите `mcu: "STM32..."` вручную.
    * Укажите `sources`: папки с исходниками (например, `Core`) или отдельные файлы.
    * Выберите необходимые `hal_components`.

**Важно про папки в `sources`:** Если вы указываете папку (например, `Core`), внутри неё должен быть свой `CMakeLists.txt`, который создает библиотеку. Если вы используете структуру CubeMX, вам, возможно, придется создать эти файлы вручную или перечислить все `.c` файлы в списке `sources` поштучно.

---

## 🖥 Настройки VS Code (.vscode)

В репозитории уже настроены файлы для комфортной работы. Они находятся в папках проектов, но базируются на общих шаблонах.

### `launch.json` (Отладка)

Содержит готовые профили для отладки через **Cortex-Debug**:

* **Debug (qemu):** Для запуска в эмуляторе (удобно для тестов без железа). Поддерживает выбор машины (Netduino, BluePill и др.) через `inputs`.
* **Debug (ocd/stlink):** Для работы с реальным железом через ST-Link и OpenOCD.
* **Debug (jlink):** Для пользователей Segger J-Link.
* **Debug (pyocd):** Альтернативный сервер отладки.

### `tasks.json` (Задачи)

Содержит задачи для автоматизации:

* `CMake: build`: Сборка проекта.
* `Запустить qemu-arm`: Запуск эмулятора с поддержкой semihosting (вывод `printf` в консоль).
* `Прошить (st-flash/j-link)`: Задачи для прошивки МК без отладки.

### `settings.json`

Настраивает пути к инструментам (OpenOCD, GDB) в зависимости от ОС (Windows/Linux). Вам может потребоваться подправить пути, если они отличаются от стандартных путей установки Scoop/apt.

---

## Лицензия

Проект распространяется под лицензией MIT. Подробности в файле `LICENSE`.

---

## 📝 Список расширений VS Code, которые могут пригодиться инженерам

ms-vscode.cpptools,
ms-vscode.cmake-tools,
ms-vscode.makefile-tools,
ms-vscode.hexeditor,
keroc.hex-fmt,
VisualStudioExptTeam.vscodeintellicode,
marus25.cortex-debug,
marus25.cortex-debug-dp-stm32f1,
marus25.cortex-debug-dp-stm32f4,
trond-snekvik.gnu-mapfiles,
yzhang.markdown-all-in-one,
streetsidesoftware.code-spell-checker,
streetsidesoftware.code-spell-checker-russian,
dionmunk.vscode-notes,
gruntfuggly.todo-tree,
rioj7.command-variable,
mcu-debug.memory-view,
webfreak.debug,
christian-kohler.path-intellisense,
eclipse-cdt.peripheral-inspector,
mcu-debug.peripheral-viewer,
donjayamanne.python-extension-pack,
magicstack.MagicPython,
ms-iot.vscode-ros,
ms-vscode.vscode-serial-monitor,
dsebastien.vscode-python-pack,
jeff-hykin.better-cpp-syntax,
aaron-bond.better-comments,
alefragnani.Bookmarks,
PKief.material-icon-theme,
emmanuelbeziat.vscode-great-icons,
johnpapa.vscode-peacock,
alefragnani.project-manager,
wayou.vscode-todo-highligh
