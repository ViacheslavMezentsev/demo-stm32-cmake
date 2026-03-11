/*!
 * \brief   Пример измерения частоты и длительности импульса методом Input Capture.
 *
 * \details Таймер TIM1 настроен на захват входного сигнала по двум каналам:
 *          - CH1: захват по обоим фронтам для измерения периода
 *          - CH2: захват по восходящему фронту для измерения длительности импульса
 *          Результаты доступны через глобальные переменные frequency_hz и duty_cycle_percent.
 *          Для проверки: подать прямоугольный сигнал на PA8 (TIM1_CH1) и PA9 (TIM1_CH2).
 *
 * \board   WeAct BlackPill (STM32F411CEU6)
 */

#include <stdint.h>
#include "main.h"
#include "stm32f4xx_it.h"

// ---------------------------------------------------------------------------

// Внешние дескрипторы периферии, инициализированные в main.c (CubeMX).
extern TIM_HandleTypeDef htim1;

// ---------------------------------------------------------------------------
// Глобальные переменные для хранения результатов измерений

/*! \brief Измеренная частота сигнала в Гц (0, если измерение не выполнено). */
volatile uint32_t frequency_hz = 0;

/*! \brief Длительность импульса в мкс (0, если измерение не выполнено). */
volatile uint32_t pulse_width_us = 0;

/*! \brief Коэффициент заполнения в процентах (0..100). */
volatile uint8_t duty_cycle_percent = 0;

// ---------------------------------------------------------------------------
// Внутренние переменные для вычислений

/// Частота тактирования таймера (определяется из RCC: APB2 = 100 МГц).
#define TIMER_CLOCK_HZ ( 100000000UL )

/// Предделитель таймера (из CubeMX: Prescaler = 0 для максимального разрешения).
#define TIMER_PRESCALER ( 0UL )

/// Частота счета таймера в Гц.
#define TIMER_COUNTER_HZ ( TIMER_CLOCK_HZ / ( TIMER_PRESCALER + 1 ) )

/// Период одного счета таймера в микросекундах.
#define TIMER_TICK_US ( 1000000UL / TIMER_COUNTER_HZ )

/// Максимальное значение для обнаружения переполнения (порог "зависания").
#define CAPTURE_TIMEOUT ( 0xFFFFU )

// Флаги готовности данных
volatile uint8_t capture_ready = 0;

// Переменные для хранения значений захвата
volatile uint16_t capture_ch1_rising  = 0;
volatile uint16_t capture_ch1_falling = 0;
volatile uint16_t capture_ch2_rising  = 0;
volatile uint16_t capture_ch2_falling = 0;

// Состояние автомата захвата
volatile uint8_t ch1_state = 0;    // 0 = ждём восходящий, 1 = ждём нисходящий
volatile uint8_t ch2_state = 0;

// ---------------------------------------------------------------------------

/*!
 * \brief Обработчик прерывания TIM1 (вызывается из stm32f4xx_it.c).
 *
 * \details Обрабатывает события захвата для обоих каналов:
 *          - CH1: измеряет период сигнала (восходящий -> нисходящий фронт)
 *          - CH2: измеряет длительность импульса (восходящий -> нисходящий фронт)
 */
void TIM1_InputCapture_Callback( void )
{
    uint32_t period_ticks;
    uint32_t pulse_ticks;

    // Обработка канала 1 (измерение периода)
    if ( __HAL_TIM_GET_FLAG( &htim1, TIM_FLAG_CC1 ) != RESET )
    {
        __HAL_TIM_CLEAR_FLAG( &htim1, TIM_FLAG_CC1 );

        if ( ch1_state == 0 )
        {
            // Зафиксирован восходящий фронт
            capture_ch1_rising = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_1 );
            ch1_state          = 1;
        }
        else
        {
            // Зафиксирован нисходящий фронт
            capture_ch1_falling = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_1 );

            // Вычисление периода с учётом переполнения
            if ( capture_ch1_falling >= capture_ch1_rising )
            {
                period_ticks = capture_ch1_falling - capture_ch1_rising;
            }
            else
            {
                // Было переполнение таймера
                period_ticks = ( 0xFFFFU - capture_ch1_rising ) + capture_ch1_falling;
            }

            // Вычисление частоты
            if ( period_ticks > 0 && period_ticks < CAPTURE_TIMEOUT )
            {
                frequency_hz = TIMER_COUNTER_HZ / period_ticks;
            }
            else
            {
                frequency_hz = 0;
            }

            ch1_state     = 0;
            capture_ready = 1;
        }
    }

    // Обработка канала 2 (измерение длительности импульса)
    if ( __HAL_TIM_GET_FLAG( &htim1, TIM_FLAG_CC2 ) != RESET )
    {
        __HAL_TIM_CLEAR_FLAG( &htim1, TIM_FLAG_CC2 );

        if ( ch2_state == 0 )
        {
            // Зафиксирован восходящий фронт
            capture_ch2_rising = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_2 );
            ch2_state          = 1;
        }
        else
        {
            // Зафиксирован нисходящий фронт
            capture_ch2_falling = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_2 );

            // Вычисление длительности импульса с учётом переполнения
            if ( capture_ch2_falling >= capture_ch2_rising )
            {
                pulse_ticks = capture_ch2_falling - capture_ch2_rising;
            }
            else
            {
                // Было переполнение таймера
                pulse_ticks = ( 0xFFFFU - capture_ch2_rising ) + capture_ch2_falling;
            }

            // Преобразование в микросекунды
            pulse_width_us = ( uint32_t ) ( ( uint64_t ) pulse_ticks * TIMER_TICK_US );

            ch2_state = 0;
        }
    }
}

// ---------------------------------------------------------------------------

/*!
 * \brief Инициализация до SystemClock_Config().
 */
void init( void )
{
    // Резервировано для инициализации до тактирования
}

/*!
 * \brief Настройка после инициализации периферии CubeMX.
 *
 * \details Настраивает таймер TIM1 в режим Input Capture:
 *          - CH1: захват по обоим фронтам (измерение периода)
 *          - CH2: захват по восходящему фронту (измерение импульса)
 *          - Включает прерывания от каналов захвата
 */
void setup( void )
{
    // Конфигурация TIM1 для Input Capture выполняется в MX_TIM1_Init()
    // Здесь только включаем прерывания и запускаем таймер

    // Включаем прерывания от каналов захвата
    __HAL_TIM_ENABLE_IT( &htim1, TIM_IT_CC1 );
    __HAL_TIM_ENABLE_IT( &htim1, TIM_IT_CC2 );

    // Запускаем таймер
    HAL_TIM_IC_Start_IT( &htim1, TIM_CHANNEL_1 );
    HAL_TIM_IC_Start_IT( &htim1, TIM_CHANNEL_2 );

    // Мигаем светодиодом для индикации запуска
    HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_SET );
}

/*!
 * \brief Тело основного цикла.
 *
 * \details В цикле обрабатываются готовые данные измерений:
 *          - Вычисляется коэффициент заполнения (duty cycle)
 *          - Сбрасывается флаг готовности
 *
 * \note    Для отладки можно добавить вывод через UART/ITM
 */
void loop( void )
{
    if ( capture_ready )
    {
        capture_ready = 0;

        // Вычисление коэффициента заполнения
        if ( frequency_hz > 0 && pulse_width_us > 0 )
        {
            // Период в микросекундах
            uint32_t period_us = 1000000UL / frequency_hz;

            if ( period_us > 0 )
            {
                // Duty cycle = (pulse_width / period) * 100
                duty_cycle_percent = ( uint8_t ) ( ( pulse_width_us * 100UL ) / period_us );

                // Ограничение диапазона
                if ( duty_cycle_percent > 100 )
                {
                    duty_cycle_percent = 100;
                }
            }
        }

        // Здесь можно добавить обработку результатов:
        // - Вывод через UART
        // - Обновление дисплея
        // - Передача по беспроводному интерфейсу
    }

    // Небольшая задержка для снижения нагрузки на CPU
    for ( volatile uint32_t i = 0; i < 1000; i++ )
    {
        __NOP();
    }
}
