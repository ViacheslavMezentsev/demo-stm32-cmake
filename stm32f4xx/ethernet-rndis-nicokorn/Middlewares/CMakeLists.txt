cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ST/STM32_USB_Device_Library/Core/Inc
    ST/STM32_USB_Device_Library/Class/CDC/Inc
    Third_Party/RNDIS
    Third_Party/printf-master/
    #Third_Party/FreeRTOS/Source/include
    #Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    #Third_Party/FreeRTOS/Source/portable/IAR/ARM_CM4F
    Third_Party/FreeRTOS-Plus-TCP/include
    Third_Party/FreeRTOS-Plus-TCP/portable/Compiler/GCC
)

target_sources(${PROJECT_NAME} PRIVATE
    #ST/STM32_USB_Device_Library/Core/Src/usbd_conf_template.c
    #ST/STM32_USB_Device_Library/Core/Src/usbd_desc_template.c
    ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c    
    ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c      

    Third_Party/RNDIS/usbd_rndis.c

    #Third_Party/FreeRTOS/Source/croutine.c
    #Third_Party/FreeRTOS/Source/event_groups.c
    #Third_Party/FreeRTOS/Source/list.c
    #Third_Party/FreeRTOS/Source/queue.c
    #Third_Party/FreeRTOS/Source/stream_buffer.c
    #Third_Party/FreeRTOS/Source/tasks.c        
    #Third_Party/FreeRTOS/Source/timers.c    
    #Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c    
    #Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c    
    #Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c

    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_ARP.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_DHCP.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_DNS.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_IP.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_Sockets.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_Stream_Buffer.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_TCP_IP.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_TCP_WIN.c
    Third_Party/FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.c
    
    #Third_Party/FreeRTOS-Plus-TCP/portable/BufferManagement/BufferAllocation_1.c
    Third_Party/FreeRTOS-Plus-TCP/portable/BufferManagement/BufferAllocation_2.c
    Third_Party/FreeRTOS-Plus-TCP/portable/NetworkInterface/HAL_RNDIS_STM32F411/NetworkInterface.c

    Third_Party/printf-master/printf.c
)
