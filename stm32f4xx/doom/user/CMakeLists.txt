cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_sources(${PROJECT_NAME} PRIVATE 
    src/button.c 
    src/debug.c 
    src/font.c 
    src/gfx.c 
    src/i2c.c 
    src/images.c 
    src/jpeg.c 
    src/lcd.c 
    src/led.c 
    src/main.c 
    src/sdram.c 
    src/spi.c 
    src/syscalls.c 
    src/touch.c 
    src/vectors.c
)
