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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
inline void pc14_init(void)
{
  // Load required values in registers using assembly instructions
  asm volatile(
      "ldr r0, =0x4001100C\n" // Load GPIOC_ODR address into register r0
      "ldr r1, [r0]\n"        // Load value in the GPIOC_ODR address into register r1
      "mov r2, #0x00004000\n" // Move the value of bit mask for PC14 into register r2
  );
}

inline void toggle_v1(void)
{
  // Toggle PC14 using assembly instructions
  asm volatile(
      "eor r1, r2\n"   // XOR the last value of GPIOC_ODR (r1) with the mask (r2)
      "str r1, [r0]\n" // Store the new value (r1) back to the address of GPIOC_ODR
  );
}

inline void toggle_v2(void)
{
  // Toggle PC14 using assembly instructions
  asm volatile(
      "eors r1, r2\n"  // XOR the last value of GPIOC_ODR (r1) with the mask (r2)
      "str r1, [r0]\n" // Store the new value (r1) back to the address of GPIOC_ODR
  );
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  pc14_init();

  while (1)
  {
    toggle_v2();

    // Frequency of the square wave generated on PC14 using toggle_v1():
    // With HSE = 8MHz :
    // - RCC_PLL_MUL9 => f_HCLK = 72 MHz => f_PC14 = 5.15 MHz (stable)
    // - RCC_PLL_MUL10 => f_HCLK = 80 MHz => f_PC14 = 5.75 MHz (stable)
    // - RCC_PLL_MUL11 => f_HCLK = 88 MHz => f_PC14 = 6.25 MHz (stable)
    // - RCC_PLL_MUL12 => f_HCLK = 96 MHz => f_PC14 = 6.85 MHz (stable)
    // - RCC_PLL_MUL13 => f_HCLK = 104 MHz => f_PC14 = 7.4 MHz (stable)
    // - RCC_PLL_MUL14 => f_HCLK = 112 MHz => f_PC14 = 8.0 MHz (stable)
    // - RCC_PLL_MUL15 => f_HCLK = 120 MHz => f_PC14 = 8.5 MHz (stable)
    // - RCC_PLL_MUL16 => f_HCLK = 128 MHz => f_PC14 = 9.1 MHz (stable)
    // With HSE = 16MHz :
    // - RCC_PLL_MUL9 => f_HCLK = 144 MHz => f_PC14 = 10.3 MHz (stable) => MAXIMUM STABLE FREQUENCY
    // - RCC_PLL_MUL10 => f_HCLK = 160 MHz => f_PC14 = 11.5 MHz (unstable: stops about 20 seconds after startup)

    // Frequency of the square wave generated on PC14 using toggle_v2():
    // With HSE = 8MHz :
    // - RCC_PLL_MUL16 => f_HCLK = 128 MHz => f_PC14 = 10.6 MHz (stable)
    // With HSE = 16MHz :
    // - RCC_PLL_MUL9 => f_HCLK = 144 MHz => f_PC14 = 12.0 MHz (stable) => MAXIMUM STABLE FREQUENCY
    // - RCC_PLL_MUL10 => f_HCLK = 160 MHz => f_PC14 = 13.3 MHz (unstable: stops about 20 seconds after startup)

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
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
