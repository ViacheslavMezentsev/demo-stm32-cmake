/*!
 * \file    program.cpp
 * \brief   Пример 07-pwm: генерация ШИМ-сигнала на TIM1 CH1 (PA8).
 *
 * \details Частота PWM: 1 кГц (APB2_TIM = 100 МГц, PSC = 99, ARR = 999).
 *          Скважность плавно меняется от 0 % до 100 % и обратно (fade-эффект).
 *          Период шага: 2 мс → полный цикл ≈ 2 × 1000 мс = 2 с.
 *
 *          Проверка логическим анализатором:
 *            - подключить щуп к PA8;
 *            - измерить частоту (должна быть стабильно 1 кГц);
 *            - наблюдать плавное изменение duty cycle от 0 % до 100 %.
 *
 * \board   WeAct BlackPill (STM32F411CEU6)
 */

#include "main.h"

/// Внешний дескриптор TIM1, инициализированный в main.c (CubeMX).
extern TIM_HandleTypeDef htim1;

// ---------------------------------------------------------------------------

/*!
 * \namespace Pwm
 * \brief Константы конфигурации ШИМ-канала.
 */
namespace Pwm
{

/// Канал таймера, подключённый к PA8.
constexpr uint32_t Channel = TIM_CHANNEL_1;

/// Верхняя граница счётчика (ARR = Period в CubeMX).
constexpr uint32_t MaxPulse = 999U;

/// Шаг изменения pulse за одну итерацию (единицы счётчика).
constexpr uint32_t Step = 1U;

/// Задержка между шагами, мс. При Step = 1 и ARR = 999: цикл ≈ 2 с.
constexpr uint32_t StepDelayMs = 2U;

}  // namespace Pwm

// ---------------------------------------------------------------------------

/// Текущее значение pulse fade-эффекта.
static uint32_t sPulse = 0U;

/// Направление изменения: \c true — нарастание, \c false — убывание.
static bool sDirection = true;

// ---------------------------------------------------------------------------

static void SetDutyCycle( uint32_t pulse );

// ---------------------------------------------------------------------------

/*!
 * \brief Инициализация до SystemClock_Config().
 *
 * \details Вызывается из main() до конфигурации тактирования.
 *          Подходит для инициализации структур данных,
 *          не зависящих от тактирования периферии.
 */
void init( void )
{
    // Резервировано.
}

/*!
 * \brief Настройка после инициализации периферии CubeMX.
 *
 * \details Запускает генерацию ШИМ на канале TIM1 CH1 (PA8).
 */
void setup( void )
{
    HAL_TIM_PWM_Start( &htim1, Pwm::Channel );
}

/*!
 * \brief Один шаг fade-эффекта, выполняется в теле основного цикла.
 *
 * \details Обновляет скважность ШИМ и выжидает StepDelayMs мс.
 *          Направление меняется автоматически при достижении границ диапазона.
 */
void loop( void )
{
    SetDutyCycle( sPulse );
    HAL_Delay( Pwm::StepDelayMs );

    if ( sDirection )
    {
        if ( sPulse >= Pwm::MaxPulse )
        {
            sDirection = false;
        }
        else
        {
            sPulse += Pwm::Step;
        }
    }
    else
    {
        if ( sPulse == 0U )
        {
            sDirection = true;
        }
        else
        {
            sPulse -= Pwm::Step;
        }
    }
}

/*!
 * \brief   Устанавливает значение сравнения CCR1 напрямую.
 *
 * \details Эффективнее повторного вызова HAL_TIM_PWM_ConfigChannel().
 *
 * \param[in] pulse  Новое значение CCR1 (0 … Pwm::MaxPulse).
 */
static void SetDutyCycle( uint32_t pulse )
{
    __HAL_TIM_SET_COMPARE( &htim1, Pwm::Channel, pulse );
}
