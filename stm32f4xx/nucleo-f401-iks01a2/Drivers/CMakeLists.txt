cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC 
    BSP/Components/Common
    BSP/Components/hts221
    BSP/Components/lps22hb
    BSP/Components/lsm6dsl
    BSP/Components/lsm303agr
    BSP/IKS01A2
    BSP/STM32F4xx_Nucleo

    CMSIS/Device/ST/STM32F4xx/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
)

target_sources(${PROJECT_NAME} PRIVATE 
    BSP/Components/hts221/hts221_reg.c
    BSP/Components/hts221/hts221.c
    BSP/Components/lps22hb/lps22hb_reg.c
    BSP/Components/lps22hb/lps22hb.c
    BSP/Components/lsm6dsl/lsm6dsl_reg.c
    BSP/Components/lsm6dsl/lsm6dsl.c
    BSP/Components/lsm303agr/lsm303agr_reg.c
    BSP/Components/lsm303agr/lsm303agr.c

    BSP/IKS01A2/iks01a2_env_sensors.c
    BSP/IKS01A2/iks01a2_env_sensors_ex.c
    BSP/IKS01A2/iks01a2_motion_sensors.c
    BSP/IKS01A2/iks01a2_motion_sensors_ex.c

    BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.c
)
