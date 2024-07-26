cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC uip )

target_sources(${PROJECT_NAME} PRIVATE
    uip/psock.c
    uip/uip.c
    uip/uip-fw.c
    uip/uip-neighbor.c
    #uip/uip-split.c
    uip/uip_arp.c
    uip/uiplib.c
)
