#include "../../interpreter.h"
#include "../../picoc.h"

#include "hardware.h"

extern volatile unsigned long PlatformDelayMs;
extern volatile unsigned long PlatformMsTicks;

void PlatformLibraryInitEx( Picoc* pc )
{
    /* Define pin constants */
    VariableDefinePlatformVar( pc, NULL, "PA0", &pc->UnsignedShortType, ( union AnyValue* ) &PA0, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA1", &pc->UnsignedShortType, ( union AnyValue* ) &PA1, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA2", &pc->UnsignedShortType, ( union AnyValue* ) &PA2, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA3", &pc->UnsignedShortType, ( union AnyValue* ) &PA3, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA4", &pc->UnsignedShortType, ( union AnyValue* ) &PA4, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA5", &pc->UnsignedShortType, ( union AnyValue* ) &PA5, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA6", &pc->UnsignedShortType, ( union AnyValue* ) &PA6, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA7", &pc->UnsignedShortType, ( union AnyValue* ) &PA7, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA8", &pc->UnsignedShortType, ( union AnyValue* ) &PA8, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA9", &pc->UnsignedShortType, ( union AnyValue* ) &PA9, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA10", &pc->UnsignedShortType, ( union AnyValue* ) &PA10, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA11", &pc->UnsignedShortType, ( union AnyValue* ) &PA11, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA12", &pc->UnsignedShortType, ( union AnyValue* ) &PA12, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA13", &pc->UnsignedShortType, ( union AnyValue* ) &PA13, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA14", &pc->UnsignedShortType, ( union AnyValue* ) &PA14, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PA15", &pc->UnsignedShortType, ( union AnyValue* ) &PA15, FALSE );

    VariableDefinePlatformVar( pc, NULL, "PB0", &pc->UnsignedShortType, ( union AnyValue* ) &PB0, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB1", &pc->UnsignedShortType, ( union AnyValue* ) &PB1, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB2", &pc->UnsignedShortType, ( union AnyValue* ) &PB2, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB3", &pc->UnsignedShortType, ( union AnyValue* ) &PB3, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB4", &pc->UnsignedShortType, ( union AnyValue* ) &PB4, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB5", &pc->UnsignedShortType, ( union AnyValue* ) &PB5, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB6", &pc->UnsignedShortType, ( union AnyValue* ) &PB6, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB7", &pc->UnsignedShortType, ( union AnyValue* ) &PB7, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB8", &pc->UnsignedShortType, ( union AnyValue* ) &PB8, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB9", &pc->UnsignedShortType, ( union AnyValue* ) &PB9, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB10", &pc->UnsignedShortType, ( union AnyValue* ) &PB10, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB11", &pc->UnsignedShortType, ( union AnyValue* ) &PB11, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB12", &pc->UnsignedShortType, ( union AnyValue* ) &PB12, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB13", &pc->UnsignedShortType, ( union AnyValue* ) &PB13, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB14", &pc->UnsignedShortType, ( union AnyValue* ) &PB14, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PB15", &pc->UnsignedShortType, ( union AnyValue* ) &PB15, FALSE );

    VariableDefinePlatformVar( pc, NULL, "PC0", &pc->UnsignedShortType, ( union AnyValue* ) &PC0, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC1", &pc->UnsignedShortType, ( union AnyValue* ) &PC1, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC2", &pc->UnsignedShortType, ( union AnyValue* ) &PC2, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC3", &pc->UnsignedShortType, ( union AnyValue* ) &PC3, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC4", &pc->UnsignedShortType, ( union AnyValue* ) &PC4, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC5", &pc->UnsignedShortType, ( union AnyValue* ) &PC5, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC6", &pc->UnsignedShortType, ( union AnyValue* ) &PC6, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC7", &pc->UnsignedShortType, ( union AnyValue* ) &PC7, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC8", &pc->UnsignedShortType, ( union AnyValue* ) &PC8, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC9", &pc->UnsignedShortType, ( union AnyValue* ) &PC9, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC10", &pc->UnsignedShortType, ( union AnyValue* ) &PC10, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC11", &pc->UnsignedShortType, ( union AnyValue* ) &PC11, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC12", &pc->UnsignedShortType, ( union AnyValue* ) &PC12, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC13", &pc->UnsignedShortType, ( union AnyValue* ) &PC13, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC14", &pc->UnsignedShortType, ( union AnyValue* ) &PC14, FALSE );
    VariableDefinePlatformVar( pc, NULL, "PC15", &pc->UnsignedShortType, ( union AnyValue* ) &PC15, FALSE );
}

void PlatformLibrarySetupEx( Picoc* pc ) {}

void PlatformSleepEx( void )
{
    __NOP();
}

void CdelayEx( unsigned long delay )
{
    PlatformDelayMs = delay;
    while ( PlatformDelayMs != 0 )
    {
        PlatformSleepEx();
    }
}

typedef struct {
    uint32_t Mode;
    uint32_t Speed;
} PinMode_t;

const PinMode_t PinMode0 = { GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW };
const PinMode_t PinMode1 = { GPIO_MODE_INPUT, GPIO_SPEED_FREQ_LOW };
const PinMode_t PinMode2 = { GPIO_MODE_IT_FALLING, GPIO_SPEED_FREQ_LOW };
const PinMode_t PinMode4 = { GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_HIGH };
const PinMode_t PinMode5 = { GPIO_MODE_OUTPUT_OD, GPIO_SPEED_FREQ_HIGH };
const PinMode_t PinMode13 = { GPIO_MODE_AF_PP, GPIO_SPEED_FREQ_HIGH };
const PinMode_t PinMode14 = { GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_HIGH };

const PinMode_t* PinModes[] = { &PinMode0, &PinMode1, &PinMode2, &PinMode2, &PinMode4, &PinMode4, &PinMode5, &PinMode5,
    &PinMode5, &PinMode5, &PinMode5, &PinMode5, &PinMode5, &PinMode13, &PinMode14 };


/// \brief Структура для хранения номера порта и пина
/// \details Используется для упрощения работы с GPIO, где порт и пин объединены в одно значение.
/// \note Порт обозначается цифрой от 0 до 3 (A = 0, B = 1, C = 2, D = 3), пин - от 0 до 15.
typedef union GPIO_Pair
{
	struct
	{
		uint8_t Port; // 0-3
		uint8_t Pin;  // 0-15
	};
	uint16_t Value; // 0-63
} GPIO_Pair_t;


/**
 *	\brief Возвращает указатель на порт GPIO по номеру пина
 *	\param pin	номер пина
 *	\return Указатель на структуру GPIO_TypeDef, соответствующую порту 	
 */
GPIO_TypeDef* GPIO_GetPort( GPIO_Pair_t pin )
{
    GPIO_TypeDef* ports[] = { GPIOA, GPIOB, GPIOC, GPIOD };

    return ( GPIO_TypeDef* ) ports[ pin.Port ];
}


/**
 *	\brief 		Устанавливает режим и значение пина
 *	\param pin 		пара порт-пин
 *	\param mode 	режим пина (0-14)
 *	\param value 	значение пина (GPIO_PinState)
 */
void CpinModeEx( int pin, int mode, int value )
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    PinMode_t modex = PinMode0;

	// Ограничиваем значение до 63 (0-63).
	GPIO_Pair_t pin_pair = { .Value = pin & 0x3F };

    // Приводим значение к типу GPIO_PinState.
    // Это нужно для совместимости с HAL, где GPIO_PinState - это enum с двумя значениями: GPIO_PIN_RESET и GPIO_PIN_SET.
    // В Picoc мы используем 0 и 1, поэтому просто приводим значение.
	value = ( GPIO_PinState ) value;

	// Проверяем, что режим в допустимых пределах.
	if ( mode < GPIO_MODE_ANALOG || mode >= ( sizeof( PinModes ) / sizeof( PinMode_t* ) ) )
	{
		// Установим режим по умолчанию.
		mode = GPIO_MODE_ANALOG;
	}

	// Устанавливаем режим и скорость пина.
	modex = *PinModes[mode];

	// Если режим - прерывание по спадающему фронту, то устанавливаем его на восходящий фронт, если значение не 0.
	// Это нужно для совместимости с HAL.
	if ( mode >= GPIO_MODE_ANALOG && mode < ( sizeof( PinModes ) / sizeof( PinMode_t* ) ) )
    {
        modex.Mode = PinModes[mode]->Mode;
        modex.Speed = PinModes[mode]->Speed;

		// Если режим - прерывание по спадающему фронту, то устанавливаем его на восходящий фронт, если значение не 0.
        if ( modex.Mode == GPIO_MODE_IT_FALLING && value != GPIO_PIN_RESET ) modex.Mode = GPIO_MODE_IT_RISING;
    }

    GPIO_InitStruct.Pin = pin_pair.Pin;
    GPIO_InitStruct.Mode = modex.Mode;
    GPIO_InitStruct.Speed = modex.Speed;

    HAL_GPIO_Init( GPIO_GetPort( pin_pair ), &GPIO_InitStruct );
}


/**
 *	\brief 		Устанавливает значение пина
 *	\param pin 		пара порт-пин
 *	\param value 	значение пина (GPIO_PinState)
 */
void CpinOutEx( int pin, int value )
{
	// Ограничиваем значение до 63 (0-63).
	GPIO_Pair_t pin_pair = { .Value = pin & 0x3F };
	
    HAL_GPIO_WritePin( GPIO_GetPort( pin_pair ), pin_pair.Pin, ( GPIO_PinState ) value );
}


/**
 *	\brief      Переключает состояние пина
 *	\param pin  пара порт-пин
 */
void CpinToggleEx( int pin )
{
	// Ограничиваем значение до 63 (0-63).
	GPIO_Pair_t pin_pair = { .Value = pin & 0x3F };

    GPIO_TypeDef* port = GPIO_GetPort( pin_pair );

    // Если порт не найден, ничего не делаем.
    if ( port == NULL )
    {
        
        return;
    }

    // Переключаем состояние пина.
    // Используем HAL_GPIO_TogglePin для совместимости с HAL.
    HAL_GPIO_TogglePin( port, pin_pair.Pin );
}

/**
 *	\brief Инициализация LCD (пока не реализовано)
 */
void ClcdInitEx( void ) {}


/**
 *	\brief Запись текста на LCD (пока не реализовано)
 *	\param text текст для отображения
 */
void ClcdWriteEx( char* text ) {}
