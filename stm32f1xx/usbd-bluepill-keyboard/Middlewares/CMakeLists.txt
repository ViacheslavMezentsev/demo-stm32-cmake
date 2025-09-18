cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ST/STM32_USB_Device_Library/Core/Inc
    ST/STM32_USB_Device_Library/Class/CDC/Inc
    ST/STM32_USB_Device_Library/Class/HID/Inc
)

target_sources(${PROJECT_NAME} PRIVATE
    #ST/STM32_USB_Device_Library/Core/Src/usbd_conf_template.c
    #ST/STM32_USB_Device_Library/Core/Src/usbd_desc_template.c
    ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c    
    ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
    ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c
)
