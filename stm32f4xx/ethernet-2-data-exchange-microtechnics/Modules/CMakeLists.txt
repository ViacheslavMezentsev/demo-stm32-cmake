cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ENC28J60)

target_sources(${PROJECT_NAME} PRIVATE ENC28J60/enc28j60.c)
