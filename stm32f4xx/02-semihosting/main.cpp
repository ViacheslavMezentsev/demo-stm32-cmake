#include <stdio.h>
#include <time.h>
#include <stm32f4xx_hal.h>
#include <core_cm4.h>
#include "version.h"

/**
 * @brief  Адрес регистра CPUID в блоке System Control Block (SCB).
 * @note   Этот адрес является стандартным для всех ядер Cortex-M.
 */
#define SCB_CPUID_ADDR ( 0xE000ED00UL )
#define SCB_CPUID      ( *( ( volatile const uint32_t* ) SCB_CPUID_ADDR ) )

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

/// Обработчик прерывания SysTick.
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}

int counter = 0;

/**
 * @brief   Разбирает и выводит в консоль информацию из регистра CPUID.
 * @note    Не зависит от HAL. Может быть вызвана в любой момент.
 */
void print_cpu_id( void )
{
    uint32_t cpuid_val = SCB_CPUID;

    // --- Извлекаем поля из 32-битного значения ---

    // Поле Implementer: [31:24]
    uint8_t implementer = ( cpuid_val >> 24 ) & 0xFF;

    // Поле Variant: [23:20]
    uint8_t variant = ( cpuid_val >> 20 ) & 0x0F;

    // Поле Constant/Architecture: [19:16]
    // 0xF для архитектуры ARMv7-M
    uint8_t architecture = ( cpuid_val >> 16 ) & 0x0F;

    // Поле Part Number: [15:4]
    uint16_t part_no = ( cpuid_val >> 4 ) & 0xFFF;

    // Поле Revision: [3:0]
    uint8_t revision = cpuid_val & 0x0F;

    // --- Выводим информацию ---

    printf( "--- CPUID Register Analysis (Value: 0x%08lX) ---\n", cpuid_val );

    // 1. Implementer (Производитель ядра)
    printf( "  Implementer [31:24]: 0x%02X -> ", implementer );
    if ( implementer == 0x41 )
    {
        printf( "ARM Ltd. ('A')\n" );
    } else if ( implementer == 0x51 )
    {
        printf( "QEMU ('Q')\n" );
    } else
    {
        printf( "Unknown\n" );
    }

    // 2. Variant (Ревизия ядра)
    printf( "  Variant     [23:20]: 0x%X   -> r%dp\n", variant, variant );

    // 3. Architecture (Архитектура)
    printf( "  Architecture[19:16]: 0x%X   -> ", architecture );

    if ( architecture == 0xF )
    {
        printf( "ARMv7-M Architecture\n" );
    }
    else if ( architecture == 0xC )
    {
        printf( "ARMv6-M Architecture\n" );
    }
    else
    {
        printf( "Unknown Architecture\n" );
    }

    // 4. Part Number (Модель ядра)
    printf( "  Part Number [15:4]:  0x%03X -> ", part_no );

    switch ( part_no )
    {
        case 0xC20:
            printf( "Cortex-M0\n" );
            break;
        case 0xC60:
            printf( "Cortex-M0+\n" );
            break;
        case 0xC21:
            printf( "Cortex-M1\n" );
            break;
        case 0xC23:
            printf( "Cortex-M3\n" );
            break;
        case 0xC24:
            printf( "Cortex-M4\n" );
            break;
        case 0xC27:
            printf( "Cortex-M7\n" );
            break;
        case 0xD20:
            printf( "Cortex-M23\n" );
            break;
        case 0xD21:
            printf( "Cortex-M33\n" );
            break;
        default:
            printf( "Unknown Core\n" );
            break;
    }

    // 5. Revision (Патч ревизии)
    printf( "  Revision    [3:0]:   0x%X   -> p%d\n", revision, revision );

    printf( "-------------------------------------------------------\n" );
}

void print_firmware_info( void )
{
    // Блок вывода версий.
    printf( "--- Firmware build information ------------------------\n" );

    // 1. Версия компилятора GCC.
    printf( "  Compiler:    GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );

    // 2. Версия CMSIS Core.
    // Эти макросы определены в файле 'core_cm3.h' (или аналогичном для вашего ядра).
    printf( "  CMSIS Core:  v%d.%d\n", __CM4_CMSIS_VERSION_MAIN, __CM4_CMSIS_VERSION_SUB );

    // 3. Версия CMSIS Device (специфично для вендора, в нашем случае ST).
    // Эти макросы определены в файле 'stm32f1xx.h'.
    printf( "  CMSIS Device:  v%d.%d.%d\n",
            __STM32F4xx_CMSIS_VERSION_MAIN,
            __STM32F4xx_CMSIS_VERSION_SUB1,
            __STM32F4xx_CMSIS_VERSION_SUB2 );

    // 4. Версия библиотеки HAL.
    // Этот макрос определен в 'stm32f1xx_hal.h'.
    uint32_t hal_version = HAL_GetHalVersion();
    uint8_t hal_major = ( hal_version >> 24 ) & 0xFF;
    uint8_t hal_minor = ( hal_version >> 16 ) & 0xFF;
    uint8_t hal_patch = ( hal_version >> 8 ) & 0xFF;
    // uint8_t hal_rc = hal_version & 0xFF; // Ревизия (обычно 0)

    printf( "  STM32Cube HAL: v%d.%d.%d\n", hal_major, hal_minor, hal_patch );

    if ( 0 )
    {
        // 5. Уникальный ID устройства (если нужно).
        // Это не версия, но очень полезно для идентификации.
        uint32_t uid_word0 = HAL_GetUIDw0();
        uint32_t uid_word1 = HAL_GetUIDw1();
        uint32_t uid_word2 = HAL_GetUIDw2();

        printf( "  Device UID:    %08lX%08lX%08lX\n", uid_word2, uid_word1, uid_word0 );
    }

    // 6. Дата и время сборки (стандартные макросы препроцессора).
    printf( "  Build Date:    %s\n", __DATE__ );
    printf( "  Build Time:    %s\n", __TIME__ );

    printf( "  Version: %u.%u.%u.%u (%02u.%02u.%02u %02u:%02u:%02u)\n",
            Version.Major, Version.Minor, Version.Build, Version.Revision,
            DAY, MON, YEAR, HOUR, MIN, SEC );

    printf( "--------------------------------------------------\n" );
}

void print_host_time()
{
    time_t raw_time = time( NULL );

    if ( raw_time != -1 )
    {
        struct tm* time_info = localtime( &raw_time );

        // asctime добавляет \n в конце.
        printf( "Host time is: %s", asctime( time_info ) );
    }
}

/**
 * \brief   Точка входа в программу.
 *
 */
int main()
{
    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();

    // Инициализация библиотеки HAL.
    HAL_Init();

    print_firmware_info();

    print_cpu_id();

    print_host_time();

    while ( 1 )
    {
        HAL_Delay( 500 );

        printf( "%i: Hello from STM32!\n", counter++ );
    }
}
