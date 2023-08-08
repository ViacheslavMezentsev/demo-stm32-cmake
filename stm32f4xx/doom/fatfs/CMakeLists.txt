cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_sources(${PROJECT_NAME} PRIVATE
    src/diskio.c 
    src/fatfs.c 
    src/fatfs_sdcard.c 
    src/fatfs_usbdisk.c 
    src/ff.c
)
