cmake_minimum_required(VERSION 3.19)

add_library(Qfplib OBJECT)

add_library(Qfplib::M0::Tiny ALIAS Qfplib)

target_include_directories(Qfplib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/Qfplib-M0-tiny)

target_sources(Qfplib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/Qfplib-M0-tiny/qfplib_gcc.S
)
