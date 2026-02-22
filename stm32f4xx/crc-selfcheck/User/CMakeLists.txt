cmake_minimum_required(VERSION 3.19)

target_sources(${PROJECT_NAME} PRIVATE
    Src/syscalls.c
    Src/program.cpp
)
