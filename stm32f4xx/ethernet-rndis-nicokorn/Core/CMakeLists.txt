cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC Inc)

target_sources(${PROJECT_NAME} PRIVATE
    Src/sysmem.c
    Src/syscalls.c

    # Основная логика приложения
    Src/main.c
    Src/gpio.c
    Src/freertos.c
    Src/stm32f4xx_it.c
    Src/stm32f4xx_hal_msp.c
    Src/stm32f4xx_hal_timebase_tim.c

    # Пользовательские модули
    Src/dhcpserver.c
    Src/dnsserver.c
    Src/httpserver.c
    Src/led.c
    Src/monitor.c
    Src/queuex.c
    Src/tcpip.c
)
