/* An STM32 HAL library written for the BMP180 temperature/pressure sensor. */
/* Library by @eepj www.github.com/eepj */
#ifndef BMP180_FOR_STM32_HAL_H
#define BMP180_FOR_STM32_HAL_H

#include "main.h"

#define BMP180_I2C_ADDR       0x77
#define BMP180_I2C_TIMEOUT    1000
#define BMP180_CONTROL_REG    0xf4
#define BMP180_MSB_REG        0xf6
#define BMP180_LSB_REG        0xf7
#define BMP180_XLSB_REG       0xf8
#define BMP180_SOFT_RESET_REG 0xe0
#define BMP180_SOFT_RESET     0xb6

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BMP180_OSS
{
    BMP180_LOW,
    BMP180_STANDARD,
    BMP180_HIGH,
    BMP180_ULTRA,
} BMP180_OSS;

typedef enum BMP180_CalIndex
{
    BMP180_INDEX_AC1,
    BMP180_INDEX_AC2,
    BMP180_INDEX_AC3,
    BMP180_INDEX_AC4,
    BMP180_INDEX_AC5,
    BMP180_INDEX_AC6,
    BMP180_INDEX_B1,
    BMP180_INDEX_B2,
    BMP180_INDEX_MB,
    BMP180_INDEX_MC,
    BMP180_INDEX_MD,
} BMP180_CalIndex;

typedef struct BMP180_EEPROM
{
    short          BMP180_AC1;
    short          BMP180_AC2;
    short          BMP180_AC3;
    unsigned short BMP180_AC4;
    unsigned short BMP180_AC5;
    unsigned short BMP180_AC6;
    short          BMP180_B1;
    short          BMP180_B2;
    short          BMP180_MB;
    short          BMP180_MC;
    short          BMP180_MD;
} BMP180_EEPROM;

extern I2C_HandleTypeDef* _bmp180_ui2c;
extern BMP180_EEPROM      _bmp180_eeprom;
extern BMP180_OSS         _bmp180_oss;

extern const uint8_t BMP180_EEPROM_ADDR_MSB[11];
extern const uint8_t BMP180_EEPROM_ADDR_LSB[11];

extern const uint8_t BMP180_CMD_TEMP;
extern const uint8_t BMP180_DELAY_TEMP;
extern const uint8_t BMP180_CMD_PRES[4];
extern const uint8_t BMP180_DELAY_PRES[4];

void BMP180_Init( I2C_HandleTypeDef* hi2c );

void BMP180_SetOversampling( BMP180_OSS oss );

void BMP180_UpdateCalibrationData( void );

void    BMP180_WriteReg( uint8_t reg, uint8_t cmd );
uint8_t BMP180_ReadReg( uint8_t reg );

int32_t BMP180_GetRawTemperature( void );
int32_t BMP180_GetPressure( void );

float BMP180_GetTemperature( void );

int32_t BMP180_GetUT( void );
int32_t BMP180_GetUP( void );

#ifdef __cplusplus
}
#endif

#endif
