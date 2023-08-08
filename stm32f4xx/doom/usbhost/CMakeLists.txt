cmake_minimum_required(VERSION 3.16)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_sources(${PROJECT_NAME} PRIVATE
    src/usb_bsp.c
    src/usb_core.c
    src/usb_hcd.c
    src/usb_hcd_int.c
    src/usb_msc_host.c
    src/usbh_core.c
    src/usbh_hcs.c
    src/usbh_ioreq.c
    src/usbh_msc_bot.c
    src/usbh_msc_core.c
    src/usbh_msc_scsi.c
    src/usbh_stdreq.c
    src/usbh_usr.c
)
