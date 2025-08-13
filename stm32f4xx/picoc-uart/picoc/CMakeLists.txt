cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ports/STM32)

target_sources(${PROJECT_NAME} PRIVATE
	cstdlib/stdio.c
    cstdlib/math.c
    cstdlib/string.c
    cstdlib/stdlib.c
	cstdlib/time.c
    cstdlib/errno.c
    cstdlib/ctype.c
    cstdlib/stdbool.c
	cstdlib/unistd.c

    ports/STM32/library_stm32.c
    ports/STM32/platform_stm32.c
    ports/platform_common.c
    ports/library_common.c
    ports/syscalls.c
    ports/sysmem.c

    picoc.c
    table.c
    lex.c
    parse.c
    expression.c
    heap.c
    type.c
	variable.c
    clibrary.c
    platform.c
    include.c
    debug.c    
)
