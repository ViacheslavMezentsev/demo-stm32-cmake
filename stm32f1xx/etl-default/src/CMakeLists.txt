cmake_minimum_required(VERSION 3.16)

target_sources(${PROJECT_NAME} PRIVATE
    syscalls.c
    main.cpp
)
