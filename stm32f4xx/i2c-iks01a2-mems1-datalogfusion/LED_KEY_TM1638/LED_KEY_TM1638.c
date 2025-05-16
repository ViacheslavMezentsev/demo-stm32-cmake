/*
  * @file 			( фаил ):   LED_KEY_TM1638.c
  * @brief 		( описание ):  	
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
*/
#include "LED_KEY_TM1638.h"

/// массив цифр для индикатора.
uint8_t num_mas[] =
{    
    0x3F,    // 0
    0x06,    // 1
    0x5B,    // 2
    0x4F,    // 3
    0x66,    // 4
    0x6D,    // 5
    0x7D,    // 6
    0x07,    // 7
    0x7F,    // 8
    0x6F,    // 9
    0x80,    // dot
    0x40,    // -
    0x63     // deg
};

// карта расположения битов согласно разрядам индикатора (включая точку).

//==   ==//
//== 0 ==//
//==   ==//

//==   ==//		//==   ==//
//== 4 ==//		//== 1 ==//
//==   ==//		//==   ==//

//==   ==//
//== 6 ==//
//==   ==//

//==   ==//		//==   ==//
//== 5 ==//		//== 2 ==//
//==   ==//		//==   ==//

//==   ==//		//==   ==//
//== 3 ==//		//== 7 ==//
//==   ==//		//==   ==//


// Процедура управления SPI.
__INLINE static void TM1638_Select( void )
{
    HAL_GPIO_WritePin( STB_GPIO_Port, STB_Pin, GPIO_PIN_RESET );
}


// Процедура управления SPI.
__INLINE static void TM1638_Unselect( void )
{
    HAL_GPIO_WritePin( STB_GPIO_Port, STB_Pin, GPIO_PIN_SET );
}


/*
	* @brief	 ( описание ):  инициализация дисплея
	* @param	( параметры ):	яркость подсветки от 0 до 7 ( светодиодов и цифр )
	* @return  ( возвращает ):	
*/
void TM1638_init( uint8_t brightness )
{
	// Ограничиваем яркость дисплея.
    if ( brightness > 7 )
    {
        brightness = 7;
    }

    // 0x80 ( отправка команды управления )
    // второе значение сама яркость и вкл или выкл сегмента,
    // используется младшая половина байта.
	// Первые три отведены на яркость и четвертый на управление включением.
    // 0x08 	Отображение ВКЛ, ширина шим яркости 1/16
    // 0x09 	Отображение ВКЛ, ширина шим яркости 2/16
    // 0x0A		Отображение ВКЛ, ширина шим яркости 4/16
    // 0x0B		Отображение ВКЛ, ширина шим яркости 10/16
    // 0x0C		Отображение ВКЛ, ширина шим яркости 11/16
    // 0x0D		Отображение ВКЛ, ширина шим яркости 12/16
    // 0x0E		Отображение ВКЛ, ширина шим яркости 13/16
    // 0x0F		Отображение ВКЛ, ширина шим яркости 14/16
    brightness = 0x80 | 0x08 | brightness;

    TM1638_Select();

    // Включаем яркость светодиодов и семисегментного индикатора ( всего 8 градаций ).
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &brightness, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

    // Очищаем все полностью и светодиоды и семисегментные индикаторы.
    TM1638_Clear_All();
}


/*
	* @brief	 ( описание ):  включение и выключение светодиодов
	* @param	( параметры ):	первый - номер светодиода от 1 до 8
								второй - состояние LED_OFF или LED_ON
	* @return  ( возвращает ):	
*/
void TM1638_Led_OnOff( uint8_t led, LedStatus status )
{
    if ( led == 0 || led > 8 )
    {
        led = 1;
    }

    TM1638_Select();

    // Команда Запись, фиксированный адрес.
    uint8_t cmd = 0x44;

    // отправляем команду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

    // формируем адрес (адрес только четные для светодиодов).
    uint8_t buf = 0xC1 + ( led - 1 ) * 2;

    TM1638_Select();

    // отправляем адрес первым байтом.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

    buf = (uint8_t) status;

    // отправляем данные вторым байтом (чтоб зажечь светодиод нужен 0x01) первый бит припаян к светодиоду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

    TM1638_Unselect();
}


/*
	* @brief	 ( описание ):  выключаем все светодиоды
	* @param	( параметры ):	
	* @return  ( возвращает ):	
*/
void TM1638_Clear_Led( void )
{
    for ( uint8_t i = 0; i < 8; i++ )
    {
        TM1638_Led_OnOff( i, LED_OFF );
    }
}


/*
	* @brief	 ( описание ):  выключаем все семисегментные индикаторы
	* @param	( параметры ):	
	* @return  ( возвращает ):	
*/
void TM1638_Clear_SevenSegment( void )
{
    TM1638_Select();

    // Команда Запись, фиксированный адрес.
    uint8_t cmd = 0x44;

    // отправляем команду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

    for ( uint8_t i = 0; i < 8; i++ )
    {
        // формируем адрес ( адрес только не четные для семисегментные индикаторы ).
        uint8_t buf = 0xC0 + i * 2;

        TM1638_Select();

        // отправляем адрес первым байтом.
        HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

        buf = 0x00;

        // отправляем данные вторым байтом 0 чтобы все отключить.
        HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

        TM1638_Unselect();
    }
}


/*
	* @brief	 ( описание ):  очищаем все полностью и светодиоды и семисегментные индикаторы
	* @param	( параметры ):	
	* @return  ( возвращает ):	
*/
void TM1638_Clear_All( void )
{
    TM1638_Select();

    // Команда Запись, автоинкремент адреса.
    uint8_t cmd = 0x40;

    // отправляем команду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

    // формируем адрес.
    uint8_t buf = 0xC0;

    TM1638_Select();

    // отправляем адрес первым байтом.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

    buf = 0x00;

    for ( uint8_t i = 0; i < 16; i++ )
    {
        // отправляем данные вторым байтом 0 чтобы все отключить.
        HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );
    }

    TM1638_Unselect();
}


/*
	* @brief	 ( описание ):  функция формирует одно разрядное число ( 1 цифру )
								в указанном разряде ( можно совместно включить точку )
	* @param	( параметры ):	1- позиция ( разряд ) где отобразить значение от 1 до 8
								2- сама цифра от 0 до 9 ( если нужен знак минус то ставим 11 )
								3- точка ( 1 включить,  0 выключить )
	* @return  ( возвращает ):	
*/
void TM1638_sendOneDigit( uint8_t pos, uint8_t digit, uint8_t dot )
{
    if ( pos == 0 || pos > 8 )
    {
        pos = 1;
    }

    if ( digit > 12 )
    {
        digit = 11;
    }

    TM1638_Select();

    // Команда Запись, фиксированный адрес.
    uint8_t cmd = 0x44;

    // отправляем команду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

    // формируем адрес ( адрес только не четные для семисегментные индикаторы ).
    uint8_t buf = 0xC0 + ( 8 - pos ) * 2;

    TM1638_Select();

    // отправляем адрес первым байтом.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

    if ( dot )
    {
        buf = num_mas[digit] | 0x80;
    }
	else
    {
        buf = num_mas[digit];
    }

	// отправляем данные вторым байтом 0 чтобы все отключить.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &buf, 1, HAL_MAX_DELAY );

    TM1638_Unselect();
}


/*
	* @brief	 ( описание ):  функция для отображения любого числа на дисплее
	* @param	( параметры ):	строка которую нужно отобразить :
								символы цифры от 0 до 9 
								знак минус ( может быть не один )
								знак точка ( может быть не один )
								Например: "-10.56" "0.56" "-2 -2.3" "1.2.3.4." "-23-" и т.д
	* @return  ( возвращает ):	
*/
void TM1638_sendNumber( char* num )
{
    uint8_t len = strlen( num );
    uint8_t dot = 0;

    while ( len )
    {
        if ( num[len - 1] == '.' )
        {
            dot++;
        }

        len--;
    }

    if ( strlen( num ) - dot > 8 )
    {
        TM1638_sendOneDigit( 0x01, 11, 0 );
        TM1638_sendOneDigit( 0x02, 11, 0 );
        TM1638_sendOneDigit( 0x03, 11, 0 );
        TM1638_sendOneDigit( 0x04, 11, 0 );
        TM1638_sendOneDigit( 0x05, 11, 0 );
        TM1638_sendOneDigit( 0x06, 11, 0 );
        TM1638_sendOneDigit( 0x07, 11, 0 );
        TM1638_sendOneDigit( 0x08, 11, 0 );

        len = 0;
    }
	else
    {
        len = strlen( num );
    }

    uint8_t position = 1;
    dot = 0;

    while ( len )
    {
        if ( num[len - 1] == '-' )
        {
            TM1638_sendOneDigit( position, 11, dot );
            dot = 0;
            position++;
        }
		else if ( num[len - 1] == 'o' )
        {
            TM1638_sendOneDigit( position, 12, dot );
            dot = 0;
            position++;
        }
		else if ( num[len - 1] == '.' )
        {
            dot = 1;
        }
		else if ( num[len - 1] == ' ' )
        {
            position++;
        }
		else
        {
            TM1638_sendOneDigit( position, num[len - 1] - 48, dot );
            dot = 0;
            position++;
        }

        len--;

        if ( position > 8 )
        {
            break;
        }
    }
}


/*
	* @brief	 ( описание ):  функция для отображения INT целочисленного числа на дисплее
	* @param	( параметры ):	число целочисленное :
								цифры от -999 9999 до 9999 9999 
	* @return  ( возвращает ):	
*/
void TM1638_sendNumberInt( int32_t num )
{
    uint8_t position = 1;

    if ( num == 0 )
    {
        TM1638_sendOneDigit( 1, 0, 0 );

    }
	else if ( ( ( num < 0 ) && ( num / 10000000 ) ) || ( ( num > 0 ) && ( num / 100000000 ) ) )
    {
        TM1638_sendOneDigit( 0x01, 11, 0 );
        TM1638_sendOneDigit( 0x02, 11, 0 );
        TM1638_sendOneDigit( 0x03, 11, 0 );
        TM1638_sendOneDigit( 0x04, 11, 0 );
        TM1638_sendOneDigit( 0x05, 11, 0 );
        TM1638_sendOneDigit( 0x06, 11, 0 );
        TM1638_sendOneDigit( 0x07, 11, 0 );
        TM1638_sendOneDigit( 0x08, 11, 0 );
    }
	else
    {
        uint8_t minus = 0;

        if ( num < 0 )
        {
            minus = 1;
            num   = num * -1;
        }

		while ( num )
        {
            TM1638_sendOneDigit( position, num % 10, 0 );

			num = num / 10;
            position++;

            if ( position > 8 )
            {
                break;
            }
        }

        if ( minus )
        {
            TM1638_sendOneDigit( position, 11, 0 );
        }
    }
}


/*
	* @brief	 ( описание ):  функция чтения кнопок
	* @param	( параметры ):	
	* @return  ( возвращает ):	1 байт каждый бит соответствует порядковому номеру кнопки
								1- нажата 0- не нажата
*/
uint8_t TM1638_readKey( void )
{
    /*
	читаем 32 бита (всего на данной плате 8 кнопок 
	поэтому не все биты нужны, есть модели где распаяны все кнопки 
	там задействованы все биты
	  
	  S1 -  1			0000 0000 0000 0000 0000 0000 0000 0001
	  S2 -  256			0000 0000 0000 0000 0000 0001 0000 0000
	  S3 -  65536		0000 0000 0000 0001 0000 0000 0000 0000
	  S4 -  16777216	0000 0001 0000 0000 0000 0000 0000 0000
	  S5 -  16			0000 0000 0000 0000 0000 0000 0001 0000
	  S6 -  4096		0000 0000 0000 0000 0001 0000 0000 0000
	  S7 -  1048576		0000 0000 0001 0000 0000 0000 0000 0000
	  S8 -  268435456 	0001 0000 0000 0000 0000 0000 0000 0000	  
	*/

    TM1638_Select();

    // Команда Чтение.
    uint8_t cmd = 0x42;

    // отправляем команду.
    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    uint32_t key;

    // читаем 32 бита данных.
    HAL_SPI_Receive( &TM1638_SPI_HAL, ( uint8_t * ) &key, 4, HAL_MAX_DELAY );

    // после того как считали значение нужно обратно записать любое число ( нужно для тактирования ).
    cmd = 0xFF;

    HAL_SPI_Transmit( &TM1638_SPI_HAL, &cmd, 1, HAL_MAX_DELAY );

    TM1638_Unselect();

	// обнуляем переменную и в нее будем формировать биты нажатых кнопок.
    cmd = 0;

    // далее так как кнопки в разнобой, формируем для удобства в 1 байт ( 8 бит как и кол-во кнопок ).
    // формируем 0 бит
    cmd = cmd | ( key & 0x00000001 );

	//	формируем 1 бит
    cmd = cmd | ( ( key >> 7 ) & 0x00000002 );

	//	формируем 2 бит
    cmd = cmd | ( ( key >> 14 ) & 0x00000004 );

	//	формируем 3 бит
    cmd = cmd | ( ( key >> 21 ) & 0x00000008 );

	//	формируем 4 бит
    cmd = cmd | ( ( key >> 0 ) & 0x00000010 );

	//	формируем 5 бит
    cmd = cmd | ( ( key >> 7 ) & 0x00000020 );

	//	формируем 6 бит
    cmd = cmd | ( ( key >> 14 ) & 0x00000040 );

	//	формируем 7 бит
    cmd = cmd | ( ( key >> 21 ) & 0x00000080 );

    return cmd;
}
