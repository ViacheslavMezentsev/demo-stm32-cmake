cmake_minimum_required(VERSION 3.19)

add_library(Qfplib OBJECT qfplib_gcc.S)
#add_library(Qfplib STATIC qfplib_gcc.S)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )

#target_sources(${PROJECT_NAME} PRIVATE qfplib_gcc.S)
