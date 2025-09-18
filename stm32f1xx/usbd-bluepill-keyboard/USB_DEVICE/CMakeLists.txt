cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC App Target)

target_sources(${PROJECT_NAME} PRIVATE
    App/usb_device.c    
    App/usbd_desc.c
    App/usbd_cdc_if.c

    Target/usbd_conf.c
)
