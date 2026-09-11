#include "main.h"
#include <stdio.h>

const char Text1[] = "Привет из STM32! (def)\n";

// --- Настройка пинов для 74HC165 ---
#define HC165_PORT      GPIOA
#define HC165_LATCH_PIN GPIO_PIN_0    // SH/~LD
#define HC165_CLK_PIN   GPIO_PIN_1    // CLK
#define HC165_DATA_PIN  GPIO_PIN_2    // Q7 (от U1)

extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}

/**
 * \brief   Инициализация портов ввода-вывода.
 */
void initGPIO()
{
    // Включаем тактирование портов
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();    // Включаем порт A для 74HC165

    // --- Настройка светодиода ---
    HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_SET );
    GPIO_InitTypeDef GPIO_Config = {
        .Pin = LED_USER_Pin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };
    HAL_GPIO_Init( LED_USER_GPIO_Port, &GPIO_Config );

    // --- Настройка пинов 74HC165 ---
    // Исходное состояние: LATCH = 1 (режим сдвига), CLK = 0
    HAL_GPIO_WritePin( HC165_PORT, HC165_LATCH_PIN, GPIO_PIN_SET );
    HAL_GPIO_WritePin( HC165_PORT, HC165_CLK_PIN, GPIO_PIN_RESET );

    // Выходы: LATCH и CLK
    GPIO_Config.Pin = HC165_LATCH_PIN | HC165_CLK_PIN;
    GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( HC165_PORT, &GPIO_Config );

    // Вход: DATA
    GPIO_Config.Pin = HC165_DATA_PIN;
    GPIO_Config.Mode = GPIO_MODE_INPUT;
    GPIO_Config.Pull = GPIO_PULLUP;    // Подтяжка лишней не будет
    HAL_GPIO_Init( HC165_PORT, &GPIO_Config );
}

/**
 * \brief Чтение 16 бит с каскада 74HC165
 */
uint16_t read_74HC165( void )
{
    uint16_t data = 0;

    // 1. ФОРМИРОВАНИЕ СТРОБА (Загрузка параллельных данных во внутренний регистр)
    // Опускаем SH/~LD в 0, затем возвращаем в 1.
    HAL_GPIO_WritePin( HC165_PORT, HC165_LATCH_PIN, GPIO_PIN_RESET );
    __NOP();
    __NOP();
    __NOP();    // Небольшая задержка для стабильности симулятора Proteus
    HAL_GPIO_WritePin( HC165_PORT, HC165_LATCH_PIN, GPIO_PIN_SET );
    __NOP();
    __NOP();
    __NOP();

    // 2. ЧТЕНИЕ ДАННЫХ
    // Сразу после возврата LATCH в 1, на выходе Q7 УЖЕ находится нулевой бит (D7 микросхемы U1).
    // Поэтому сначала читаем, потом дергаем CLK.
    for ( int i = 0; i < 16; i++ )
    {
        data <<= 1;    // Сдвигаем прочитанные данные влево

        // Читаем текущий бит
        if ( HAL_GPIO_ReadPin( HC165_PORT, HC165_DATA_PIN ) == GPIO_PIN_SET )
        {
            data |= 1;
        }

        // Генерируем тактовый импульс (сдвиг к следующему биту)
        HAL_GPIO_WritePin( HC165_PORT, HC165_CLK_PIN, GPIO_PIN_SET );
        __NOP();
        __NOP();
        HAL_GPIO_WritePin( HC165_PORT, HC165_CLK_PIN, GPIO_PIN_RESET );
        __NOP();
        __NOP();
    }

    return data;
}

/**
 * \brief   Точка входа в программу.
 */
int main()
{
    // Инициализация библиотеки HAL.
    HAL_Init();

    // Инициализация портов ввода-вывода.
    initGPIO();

    // Настройка USART1.
    MX_USART1_UART_Init();

    // Устанавливаем функцию вывода.
    xdev_out( uart_putc );

    println( "\n--- System Started ---" );

    println( "\n--- System Clock Information (using HAL) ---" );

    print( "Target CPU Frequency (HSE_VALUE): " );
    printf( "%lu", HSE_VALUE / 1000000 );
    println( " MHz" );

    // Получаем "реальные" текущие частоты из регистров контроллера с помощью HAL-функций.
    print( "Actual SYSCLK Frequency:      " );
    printf( "%lu", HAL_RCC_GetSysClockFreq() / 1000000 );
    println( " MHz" );

    print( "HCLK (Core, AHB) Frequency:   " );
    printf( "%lu", HAL_RCC_GetHCLKFreq() / 1000000 );
    println( " MHz" );

    print( "PCLK1 (APB1) Frequency:       " );
    printf( "%lu", HAL_RCC_GetPCLK1Freq() / 1000000 );
    println( " MHz  <-- UART2, UART3 are here" );

    print( "PCLK2 (APB2) Frequency:       " );
    printf( "%lu", HAL_RCC_GetPCLK2Freq() / 1000000 );
    println( " MHz  <-- UART1 is here" );

    println( "------------------------------------------" );

    while ( 1 )
    {
        // Переключаем выход порта (мигаем светодиодом).
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

        // Выполняем чтение ровно 3 раза
        for ( int i = 1; i <= 3; i++ )
        {
            uint16_t sensor_data = read_74HC165();

            print( "Read " );
            printf( "%d", i );
            print( ": 0x" );
            printf( "%04X", sensor_data );    // Вывод в 16-ричном формате
            println( "" );

            HAL_Delay( 100 );    // Небольшая пауза между чтениями
        }

        println( "--- Wait ---" );
        HAL_Delay( 2000 );    // Ждем перед следующей серией из 3-х чтений
    }
}
