cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ENC28J60 ARP ICMP IP Ethernet)

target_sources(${PROJECT_NAME} PRIVATE
    ENC28J60/enc28j60.c
    ARP/arp.c
    ICMP/icmp.c
    IP/ip.c
    Ethernet/ethernet.c
)
