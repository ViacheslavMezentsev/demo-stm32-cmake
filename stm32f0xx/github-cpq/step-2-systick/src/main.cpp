#include <stm32f0xx_ll_gpio.h>

/// Тип дискретного вывода.
class PortPin
{
public:
    uint8_t Port;
    uint8_t Pin;

    PortPin( uint8_t port, uint8_t pin ) : Port( port - 'A' ), Pin( pin ) {}

    GPIO_TypeDef * getGPIO() { return GPIOA + 0x400 * Port; }

    void SetPinMode( uint32_t mode ) { LL_GPIO_SetPinMode( getGPIO(), LL_GPIO_PIN_0 << Pin, mode ); }

    void SetPinOutputType( uint32_t outputType ) { LL_GPIO_SetPinOutputType( getGPIO(), LL_GPIO_PIN_0 << Pin, outputType ); }

    /// Устанавливает выводы в высокий уровень для указанного порта.
    void Set() { LL_GPIO_SetOutputPin( getGPIO(), LL_GPIO_PIN_0 << Pin ); }

    /// Сбрасывает выводы в низкий уровень для указанного порта.
    void Reset() { LL_GPIO_ResetOutputPin( getGPIO(), LL_GPIO_PIN_0 << Pin ); }

    /// Переключает уровень на выводе для указанного порта.
    void Toggle() { LL_GPIO_TogglePin( getGPIO(), LL_GPIO_PIN_0 << Pin ); }
};


/**
 * \brief   Точка входа в программу.
 *
 */
int main( void )
{
    // Объединяем порт и номер вывода.
    auto led = PortPin( 'A', 5 );    

    // Включаем тактирование порта для выбранного вывода.
    SET_BIT( RCC->AHBENR, RCC_AHBENR_GPIOAEN << led.Port );

    // Настраиваем режим.
    led.SetPinMode( LL_GPIO_MODE_OUTPUT );
    led.SetPinOutputType( LL_GPIO_OUTPUT_PUSHPULL );

    led.Set();
    led.Reset();

    for ( ;; )
    {
        led.Toggle();
    }

    return 0;
}
