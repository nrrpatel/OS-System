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
#include <stdio.h>
#include "kernel.h"
uint32_t* stackptr;
uint32_t* oldstackptr;
int stackNum = 0;
int interesting_value = 50; // Define and initialize the interesting value
int sharedVariable = 0;


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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,HAL_MAX_DELAY);
return ch;
}



void jumpAssembly(void* fcn)
{
	__asm("MOV PC, R0");
}


void print_continuously(void* interesting_value) {

	int value_1 = *(int*)interesting_value;
    while (1) {
        printf("Thread1: %d\n", value_1);
        osYield();
    }
}

void print_again(void* arguments) {
	ThreadArguments* args = (ThreadArguments*)arguments;
	int value1 = args->value1;
	int value2 = args->value2;

	while (true) {
		sharedVariable = value2;
	    printf("Thread2: %d\n", sharedVariable);
	}
}

void newthing(void* interesting_value) {

	int value_1 = *(int*)interesting_value;
    while (1) {
        printf("Other Thread: %d\n", value_1);
//        osYield();
    }
}

void againthing(void* interesting_value) {

	int value_1 = *(int*)interesting_value;
    while (1) {
        printf("New: %d\n", value_1);

    }
}

// Test case thread 1: Print thread ID continuously
void print_thread_id(void* thread_id) {
    int id = *(int*)thread_id;
    while (1) {
        printf("Thread ID: %d\n", id);
    }
    osYield();
}

// Test case thread 2: Print thread ID continuously with different deadline
void print_thread_id_urgent(void* thread_id) {
    int id = *(int*)thread_id;
    while (1) {
        printf("URGENT Thread ID: %d\n", id);
    }
}

void print_thread_id_1(void* thread_id) {
    int id = *(int*)thread_id;
    while (1) {
        printf("Thread ID again: %d\n", id);
    }
    osYield();
}

// Test case thread 2: Print thread ID continuously with different deadline
void print_thread_id_urgent_1(void* thread_id) {
    int id = *(int*)thread_id;
    while (1) {
        printf("URGENT Thread ID new: %d\n", id);
    }
}

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  *
  *
  */

int main(void)
{
	int thread1_id = 1;
	int thread2_id = 2;

	ThreadArguments args = {
			.value1 = 100,
	        .value2 = 150,
	};
	void (*continuous_print_ptr)(void);  // Function pointer declaration

	continuous_print_ptr = &print_continuously;  // Assign function address to the pointer

	void (*continuous_other_print_ptr)(void);  // Function pointer declaration

	continuous_other_print_ptr = &print_again;

	void (*new_ptr)(void);  // Function pointer declaration

	new_ptr = &newthing;

	void (*other_ptr)(void);  // Function pointer declaration

	other_ptr = &againthing;
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
  MX_USART2_UART_Init();
  uint32_t* MSP_INIT_VAL = *(uint32_t**)0x0;
//  uint32_t PSP = (uint32_t)MSP_INIT_VAL - 0x200;
//  stackptr = (uint32_t*)PSP;
  oldstackptr = MSP_INIT_VAL;

//  runFirstThread();
  osKernelInitialize();

  // Create thread 1 with a deadline of 100 ms
  osThreadCreateWithDeadline(print_thread_id, &thread1_id, 100);

      // Create thread 2 with a deadline of 50 ms (more urgent)
  osThreadCreateWithDeadline(print_thread_id_urgent, &thread2_id, 200);

  osThreadCreateWithDeadline(print_thread_id_1, &thread1_id, 300);

      // Create thread 2 with a deadline of 50 ms (more urgent)
//  osThreadCreateWithDeadline(print_thread_id_urgent_1, &thread2_id, 50);

//  osThreadCreateWithDeadline(continuous_print_ptr, &args, 300);
//  osThreadCreateWithDeadline(other_ptr, &args, 100);
//  osThreadCreateWithDeadline(new_ptr, &args, 200);
//  osThreadCreateWithDeadline(continuous_other_print_ptr, &args, 400);


//  osThreadCreateWithDeadline(continuous_other_print_ptr, &args, 100);
//  osThreadCreateWithDeadline(new_ptr, &args, 200);



  osKernelStart();



//  setup_thread_stack();


  /* USER CODE BEGIN 2 */


  /* USER CODE END 2 */



//  print_continously();


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
//	 stack_call();


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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
