#include <stm32f0xx_ll_gpio.h>

#define BIT( x )        ( 1UL << ( x ) )

/// Тип дискретного вывода.
class PortPin
{
public:
    uint8_t Port;
    uint8_t Pin;

    GPIO_TypeDef * getGPIO() { return GPIOA + 0x400 * Port; }

    PortPin( uint8_t port, uint8_t pin ) : Port( port - 'A' ), Pin( pin ) {}
};

int main( void )
{
    SystemCoreClockUpdate();

    // Объединяем порт и номер вывода.
    auto led = PortPin( 'A', 5 );    

    // Включаем тактирование порта для выбранного вывода.
    SET_BIT( RCC->AHBENR, RCC_AHBENR_GPIOAEN << led.Port );

    auto gpio = led.getGPIO();
    auto pin = LL_GPIO_PIN_0 << led.Pin;

    // Настраиваем режим для выбранного вывода.
    LL_GPIO_SetPinMode( gpio, pin, LL_GPIO_MODE_OUTPUT );

    LL_GPIO_SetPinOutputType( gpio, pin, LL_GPIO_OUTPUT_PUSHPULL );

    auto value = BIT( led.Pin );

    SET_BIT( gpio->ODR, value );

    CLEAR_BIT( gpio->ODR, value );

    // Загружает слово в регистр вывода данных порта.
    LL_GPIO_WriteOutputPort( gpio, value );

    auto temp = LL_GPIO_ReadOutputPort( gpio );

    CLEAR_BIT( temp, value );

    LL_GPIO_WriteOutputPort( gpio, temp );

    // Устанавливает выводы в высокий уровень для указанного порта.
    LL_GPIO_SetOutputPin( gpio, value );

    // Сбрасывает выводы в низкий уровень для указанного порта.
    LL_GPIO_ResetOutputPin( gpio, value );

    for ( ;; )
    {
        LL_GPIO_TogglePin( gpio, value );
    }

    return 0;
}
