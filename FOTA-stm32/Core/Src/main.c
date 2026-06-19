/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "suit_parser.h"

#include "manifest_fixture.h"
#include "manifest_fixture11.h"
#include "manifest_fixture12.h"
#include "manifest_fixture13.h"
#include "manifest_fixture14.h"
#include "manifest_fixture15.h"
#include "manifest_fixture16.h"
#include "manifest_fixture17.h"
#include "manifest_fixture18.h"

#include <stdio.h>
#include <stdint.h>

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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct {
  const char *name;
  const uint8_t *data;
  size_t len;
  int expected_rc;
} parser_test_case_t;

static const char *parser_error_name(int rc)
{
  switch (rc) {
    case CBOR_ERR_NONE: return "CBOR_ERR_NONE";
    case CBOR_ERR_TYPE_MISMATCH: return "CBOR_ERR_TYPE_MISMATCH";
    case CBOR_ERR_KEY_MISMATCH: return "CBOR_ERR_KEY_MISMATCH";
    case CBOR_ERR_OVERRUN: return "CBOR_ERR_OVERRUN";
    case CBOR_ERR_INTEGER_DECODE_OVERFLOW: return "CBOR_ERR_INTEGER_DECODE_OVERFLOW";
    case CBOR_ERR_INTEGER_ENCODING: return "CBOR_ERR_INTEGER_ENCODING";
    case CBOR_ERR_UNIMPLEMENTED: return "CBOR_ERR_UNIMPLEMENTED";
    case SUIT_ERR_VERSION: return "SUIT_ERR_VERSION";
    case SUIT_ERR_SIG: return "SUIT_ERR_SIG";
    case SUIT_ERROR_DIGEST_MISMATCH: return "SUIT_ERROR_DIGEST_MISMATCH";
    case SUIT_MFST_ERR_AUTH_MISSING: return "SUIT_MFST_ERR_AUTH_MISSING";
    case SUIT_MFST_ERR_MANIFEST_ENCODING: return "SUIT_MFST_ERR_MANIFEST_ENCODING";
    case SUIT_MFST_UNSUPPORTED_ENTRY: return "SUIT_MFST_UNSUPPORTED_ENTRY";
    case SUIT_MFST_CONDITION_FAILED: return "SUIT_MFST_CONDITION_FAILED";
    case SUIT_MFST_UNSUPPORTED_COMMAND: return "SUIT_MFST_UNSUPPORTED_COMMAND";
    case SUIT_MFST_UNSUPPORTED_ARGUMENT: return "SUIT_MFST_UNSUPPORTED_ARGUMENT";
    case SUIT_MFST_ERR_VENDOR_MISMATCH: return "SUIT_MFST_ERR_VENDOR_MISMATCH";
    case SUIT_MFST_ERR_CLASS_MISMATCH: return "SUIT_MFST_ERR_CLASS_MISMATCH";
    case SUIT_ERR_PARAMETER_KEY: return "SUIT_ERR_PARAMETER_KEY";
    case SUIT_ERR_IMAGE_SIZE: return "SUIT_ERR_IMAGE_SIZE";
    default: return "unknown";
  }
}

static void run_parser_test(const parser_test_case_t *test)
{
  int rc = suit_do_process_manifest(test->data, test->len);
  const bm_cbor_err_info_t *err = bm_cbor_get_err_info();

  printf("\r\n[test] %s (%lu bytes)\r\n", test->name, (unsigned long)test->len);
  printf("[parser] %s: %s (rc=%d, expected=%d)\r\n",
         rc == test->expected_rc ? "PASS" : "FAIL",
         parser_error_name(rc), rc, test->expected_rc);

  if (rc != CBOR_ERR_NONE) {
    printf("[parser] Error source: %s:%lu\r\n",
           err->file != NULL ? err->file : "<input validation>",
           (unsigned long)err->line);
    if (err->ptr != NULL && test->data != NULL &&
        err->ptr >= test->data && err->ptr <= test->data + test->len) {
      printf("[parser] Manifest offset: %lu%s\r\n",
             (unsigned long)(err->ptr - test->data),
             err->ptr == test->data + test->len ? " (end of fixture)" : "");
    }
  }
}

/*

  @brief  printf 함수 출력을 USART1로 리다이렉션해 시리얼 모니터로 디버깅 로그를 확인
*/
int __io_putchar(int ch)
{
//	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

	if (ch == '\n') {
		const uint8_t cr = '\r';
		HAL_UART_Transmit(&huart1, &cr, 1, HAL_MAX_DELAY);
	}

	const uint8_t byte = (uint8_t)ch;
	HAL_UART_Transmit(&huart1, &byte, 1, HAL_MAX_DELAY);


	return ch;
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
  MX_USART1_UART_Init();  // 시리얼 로그 출력용

  /* USER CODE BEGIN 2 */
  const parser_test_case_t tests[] = {
	  {
		  "example1",
		  manifest_fixture,
		  manifest_fixture_len,
		  CBOR_ERR_NONE  //  파서가 실제로 반환할 것으로 예상하는 결과값
	  },
      {
          "example11: 2^32-2",
          manifest_fixture11,
          manifest_fixture11_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example12: 2^32-1",
          manifest_fixture12,
          manifest_fixture12_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example13: 2^32",
          manifest_fixture13,
          manifest_fixture13_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example14: 2^32+1",
          manifest_fixture14,
          manifest_fixture14_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example15: 2^32+4095",
          manifest_fixture15,
          manifest_fixture15_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example16: 2^32+4096",
          manifest_fixture16,
          manifest_fixture16_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example17: 2^32+34768",
          manifest_fixture17,
          manifest_fixture17_len,
          SUIT_ERR_IMAGE_SIZE
      },
      {
          "example18: 2^33",
          manifest_fixture18,
          manifest_fixture18_len,
          SUIT_ERR_IMAGE_SIZE
      },
  };

  printf("\r\n===== SUIT Parser STM32 Valid Manifest Baseline =====\r\n");
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    run_parser_test(&tests[i]);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/*
	* 파서는 부팅 후 한 번만 실행
	* 반복 파싱이 필요하면 보드 reset 또는 별도 UART 명령을 사용
	*/
	HAL_Delay(1000);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
	printf("\r\n[critical] Error_Handler entered\r\n");
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
