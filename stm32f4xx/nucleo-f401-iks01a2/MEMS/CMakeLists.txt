cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC Target App)

target_sources(${PROJECT_NAME} PRIVATE
    App/app_mems.c
)
