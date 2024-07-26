cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC
    modbus/include
    modbus/tcp
    port
)

target_sources(${PROJECT_NAME} PRIVATE
    modbus/functions/mbfunccoils.c
    modbus/functions/mbfuncdiag.c
    modbus/functions/mbfuncdisc.c
    modbus/functions/mbfuncholding.c
    modbus/functions/mbfuncinput.c
    modbus/functions/mbfuncother.c
    modbus/functions/mbutils.c
    modbus/tcp/mbtcp.c
    modbus/mb.c

    port/portevent.c
    port/porttcp.c
)
