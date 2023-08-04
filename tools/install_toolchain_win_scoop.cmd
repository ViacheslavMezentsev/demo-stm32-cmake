:: Установка scoop.
powershell Set-ExecutionPolicy RemoteSigned -Scope CurrentUser;
powershell "irm get.scoop.sh | iex"

:: Добавление репозиториев.
powershell scoop bucket add main
powershell scoop bucket add extras

::powershell scoop install windows-terminal
::powershell scoop install pwsh
::powershell scoop install gdb
::powershell scoop install make

powershell scoop install ninja
powershell scoop install cmake
powershell scoop install gcc-arm-none-eabi
powershell scoop install openocd
powershell scoop install srecord
powershell scoop install stlink

