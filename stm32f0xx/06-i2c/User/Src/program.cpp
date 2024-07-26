#include "i2c.h"
#include "main.h"

/// Период сигнала, управляющего светодиодом LED_USER, [мс].
#define LED_USER_PERIOD_MSEC    ( 500 )

/// Адрес устройства на шине I2C.
#define I2C_SLAVE_ADDRESS       ( 0x2D )

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

uint8_t masterTxData = 0x00;
uint8_t masterRxData = 0x00;
uint8_t slaveTxData  = 0x00;
uint8_t slaveRxData  = 0x00;

uint8_t slaveWaitData  = 0;
uint8_t masterWaitData = 0;

uint32_t masterTxCounter = 0;
uint32_t masterRxCounter = 0;
uint32_t slaveTxCounter  = 0;
uint32_t slaveRxCounter  = 0;

I2C_HandleTypeDef* i2cMasterHandler = &hi2c2;
I2C_HandleTypeDef* i2cSlaveHandler  = &hi2c1;

/**
 * \brief   Выполняет инициализацию.
 *
 */
void init( void ) {}


/**
 * \brief   Выполняет дополнительные настройки.
 *
 */
void setup( void )
{
    // Устанавливаем состояние по умолчанию.
    if ( HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin ) == GPIO_PIN_SET )
    {
        HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_RESET );
    }
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    slaveWaitData = 1;

    HAL_I2C_Slave_Receive_IT( i2cSlaveHandler, & slaveRxData, 1 );
    HAL_I2C_Master_Transmit_IT( i2cMasterHandler, ( I2C_SLAVE_ADDRESS << 1 ), & masterTxData, 1 );

    while ( slaveWaitData ) {};

    slaveTxData = slaveRxData + 1;

    masterWaitData = 1;

    HAL_I2C_Master_Receive_IT( i2cMasterHandler, ( I2C_SLAVE_ADDRESS << 1 ), & masterRxData, 1 );
    HAL_I2C_Slave_Transmit_IT( i2cSlaveHandler, & slaveTxData, 1 );

    while ( masterWaitData ) {};

    masterTxData = masterRxData + 1;

    HAL_Delay( 500 );
}


/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Индикация работы основного цикла.
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
    }
}


void HAL_I2C_MasterRxCpltCallback( I2C_HandleTypeDef* hi2c )
{
    if ( hi2c == i2cMasterHandler )
    {
        masterWaitData = 0;
        masterRxCounter++;
    }
}


void HAL_I2C_MasterTxCpltCallback( I2C_HandleTypeDef* hi2c )
{
    if ( hi2c == i2cMasterHandler )
    {
        masterTxCounter++;
    }
}


void HAL_I2C_SlaveRxCpltCallback( I2C_HandleTypeDef* hi2c )
{
    if ( hi2c == i2cSlaveHandler )
    {
        slaveWaitData = 0;
        slaveRxCounter++;
    }
}


void HAL_I2C_SlaveTxCpltCallback( I2C_HandleTypeDef* hi2c )
{
    if ( hi2c == i2cSlaveHandler )
    {
        slaveTxCounter++;
    }
}
