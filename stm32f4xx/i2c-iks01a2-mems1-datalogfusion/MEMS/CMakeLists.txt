cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC App Target
    "../FlashStorage_STM32"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/Common"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm6dsl"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm303agr"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lps22hb"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/hts221"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/IKS01A2"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/STM32F4xx-Nucleo"
    "${STM32_XCUBE_MEMS1_PATH}/Middlewares/ST/STM32_MotionFX_Library/Inc"
    "${STM32_XCUBE_MEMS1_PATH}/Middlewares/ST/STM32_MotionMC_Library/Inc"
)

target_sources(${PROJECT_NAME} PRIVATE
    App/app_mems.c
    Target/com.c
    Target/demo_serial.c
    Target/iks01a2_mems_control.c
    Target/iks01a2_mems_control_ex.c
    Target/motion_fx_manager.c
    Target/motion_mc_manager.c
    Target/serial_protocol.c

    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm6dsl/lsm6dsl_reg.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm6dsl/lsm6dsl.c"

    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm303agr/lsm303agr.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lsm303agr/lsm303agr_reg.c"

    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lps22hb/lps22hb.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/lps22hb/lps22hb_reg.c"

    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/hts221/hts221.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/Components/hts221/hts221_reg.c"

    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/IKS01A2/iks01a2_env_sensors.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/IKS01A2/iks01a2_env_sensors_ex.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/IKS01A2/iks01a2_motion_sensors.c"
    "${STM32_XCUBE_MEMS1_PATH}/Drivers/BSP/IKS01A2/iks01a2_motion_sensors_ex.c"
)

target_link_libraries(${PROJECT_NAME} "${STM32_XCUBE_MEMS1_PATH}/Middlewares/ST/STM32_MotionFX_Library/Lib/MotionFX_CM4F_wc32_ot_hard.a")
target_link_libraries(${PROJECT_NAME} "${STM32_XCUBE_MEMS1_PATH}/Middlewares/ST/STM32_MotionMC_Library/Lib/MotionMC_CM4F_wc32_ot_hard.a")
