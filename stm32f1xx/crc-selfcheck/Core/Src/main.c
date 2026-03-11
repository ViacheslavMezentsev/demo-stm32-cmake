/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <inttypes.h>
#include <stdio.h>
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Объявления внешних символов из скрипта компоновщика (.ld).
extern uint32_t __checksum_start[]; /// Начало проверяемой области
extern uint32_t __checksum_end[];   /// Начало секции с CRC (тут лежит записанное значение)
extern uint32_t __checksum_size[];  /// Символ, чей АДРЕС равен размеру данных

/**
 * @brief  Выполняет проверку целостности прошивки (CRC32).
 * @param  hcrc Указатель на хендлер CRC.
 * @return 1 - целостность подтверждена, 0 - ошибка.
 */
int CheckFirmwareIntegrity(CRC_HandleTypeDef *hcrc)
{
    // 1. Получаем адрес длину данных в 32-битных словах.
    uint32_t start_addr = ( uint32_t ) __checksum_start;

    // Важно: __checksum_size - это символ, его адрес равен размеру в байтах.
    uint32_t data_len_words = ( uint32_t ) __checksum_size / sizeof( uint32_t );

    // 2. Считаем CRC от начала прошивки до начала секции .checksum
    uint32_t calc_crc = HAL_CRC_Calculate( hcrc, ( uint32_t* ) __checksum_start, data_len_words );

    // 3. Читаем эталонное значение, записанное утилитой сборки.
    uint32_t stored_crc = __checksum_end[0];

    // 4. Сравниваем.
    if ( calc_crc != stored_crc )
    {
        printf( "\n\n" );
        printf( "КРИТИЧЕСКАЯ ОШИБКА: ПРОВЕРКА ЦЕЛОСТНОСТИ КОДА НЕ ПРОЙДЕНА!\n" );
        printf( "Start Addr: 0x%08" PRIX32 "\n", start_addr );
        printf( "Length:     %" PRIu32 " bytes (%" PRIu32 " words)\n", ( uint32_t ) __checksum_size, data_len_words );
        printf( "Stored CRC: 0x%08" PRIX32 " (at 0x%08" PRIX32 ")\n", stored_crc, ( uint32_t ) __checksum_end );
        printf( "Calc CRC:   0x%08" PRIX32 "\n", calc_crc );
        printf( "Система будет остановлена.\n" );

        return HAL_ERROR;
    }

    return HAL_OK;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  // Инициализация CRC.
  hcrc.Instance = CRC;

  if ( HAL_CRC_Init( &hcrc ) != HAL_OK ) Error_Handler();

  // Не запускаем основную логику сломанной программы.
  if ( CheckFirmwareIntegrity( &hcrc ) != HAL_OK )
  {
      Error_Handler();
  }
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  setup();

  while (1)
  {
    loop();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
