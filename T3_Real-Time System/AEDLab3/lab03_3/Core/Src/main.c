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
#include "cmsis_os.h"

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
TIM_HandleTypeDef htim2;

/* Definitions for led1 */
osThreadId_t led1Handle;
const osThreadAttr_t led1_attributes = {
  .name = "led1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for led2 */
osThreadId_t led2Handle;
const osThreadAttr_t led2_attributes = {
  .name = "led2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for led3 */
osThreadId_t led3Handle;
const osThreadAttr_t led3_attributes = {
  .name = "led3",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for led4 */
osThreadId_t led4Handle;
const osThreadAttr_t led4_attributes = {
  .name = "led4",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for pwm1 */
osThreadId_t pwm1Handle;
const osThreadAttr_t pwm1_attributes = {
  .name = "pwm1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pwm2 */
osThreadId_t pwm2Handle;
const osThreadAttr_t pwm2_attributes = {
  .name = "pwm2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pwm3 */
osThreadId_t pwm3Handle;
const osThreadAttr_t pwm3_attributes = {
  .name = "pwm3",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pwm4 */
osThreadId_t pwm4Handle;
const osThreadAttr_t pwm4_attributes = {
  .name = "pwm4",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */
GPIO_TypeDef *ledPort[4] = {GPIOB,GPIOC,GPIOC,GPIOB};
int ledGPIO[4] = {GPIO_PIN_2,GPIO_PIN_6,GPIO_PIN_8,GPIO_PIN_3};
int pwm[4]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void led1Thread(void *argument);
void led2Thread(void *argument);
void led3Thread(void *argument);
void led4Thread(void *argument);
void pwm1Thread(void *argument);
void pwm2Thread(void *argument);
void pwm3Thread(void *argument);
void pwm4Thread(void *argument);

/* USER CODE BEGIN PFP */
void clearTimer();
uint32_t getTimer();
void uDelay(uint32_t us);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of led1 */
  led1Handle = osThreadNew(led1Thread, NULL, &led1_attributes);

  /* creation of led2 */
  led2Handle = osThreadNew(led2Thread, NULL, &led2_attributes);

  /* creation of led3 */
  led3Handle = osThreadNew(led3Thread, NULL, &led3_attributes);

  /* creation of led4 */
  led4Handle = osThreadNew(led4Thread, NULL, &led4_attributes);

  /* creation of pwm1 */
  pwm1Handle = osThreadNew(pwm1Thread, NULL, &pwm1_attributes);

  /* creation of pwm2 */
  pwm2Handle = osThreadNew(pwm2Thread, NULL, &pwm2_attributes);

  /* creation of pwm3 */
  pwm3Handle = osThreadNew(pwm3Thread, NULL, &pwm3_attributes);

  /* creation of pwm4 */
  pwm4Handle = osThreadNew(pwm4Thread, NULL, &pwm4_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 9000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB2 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void clearTimer(){
	__HAL_TIM_SetCounter(&htim2,0);
}

uint32_t getTimer(){
	return __HAL_TIM_GetCounter(&htim2);
}

void uDelay(uint32_t us){
	uint32_t start;
	uint32_t finish = (start =__HAL_TIM_GetCounter(&htim2)) + us;
	if(start > finish){
		while(__HAL_TIM_GetCounter(&htim2)>start)
			osThreadYield(); // wait until it overruns
	}
	while(__HAL_TIM_GetCounter(&htim2)<finish)
		osThreadYield();
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_led1Thread */
/**
  * @brief  Function implementing the led1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_led1Thread */
void led1Thread(void *argument)
{
  /* USER CODE BEGIN 5 */
	int i;
  /* Infinite loop */
  for(;;)
  {
	  for(i=0;i<100;i++){
		  pwm[0]=i;
		  osDelay(1);
	  }
	  for(i=100;i>0;i--){
		  pwm[0]=i;
		  osDelay(1);
	  }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_led2Thread */
/**
* @brief Function implementing the led2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_led2Thread */
void led2Thread(void *argument)
{
  /* USER CODE BEGIN led2Thread */
	int i;
  /* Infinite loop */
  for(;;)
  {
	  for(i=0;i<100;i++){
		  pwm[1]=i;
		  osDelay(2);
	  }
	  for(i=100;i>0;i--){
		  pwm[1]=i;
		  osDelay(2);
	  }
  }
  /* USER CODE END led2Thread */
}

/* USER CODE BEGIN Header_led3Thread */
/**
* @brief Function implementing the led3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_led3Thread */
void led3Thread(void *argument)
{
  /* USER CODE BEGIN led3Thread */
	int i;
  /* Infinite loop */
  for(;;)
  {
	  for(i=0;i<100;i++){
		  pwm[2]=i;
		  osDelay(4);
	  }
	  for(i=100;i>0;i--){
		  pwm[2]=i;
		  osDelay(4);
	  }
  }
  /* USER CODE END led3Thread */
}

/* USER CODE BEGIN Header_led4Thread */
/**
* @brief Function implementing the led4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_led4Thread */
void led4Thread(void *argument)
{
  /* USER CODE BEGIN led4Thread */
	int i;
  /* Infinite loop */
  for(;;)
  {
	  for(i=0;i<100;i++){
		  pwm[3]=i;
		  osDelay(8);
	  }
	  for(i=100;i>0;i--){
		  pwm[3]=i;
		  osDelay(8);
	  }
  }
  /* USER CODE END led4Thread */
}

/* USER CODE BEGIN Header_pwm1Thread */
/**
* @brief Function implementing the pwm1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm1Thread */
void pwm1Thread(void *argument)
{
  /* USER CODE BEGIN pwm1Thread */
	int pOn,pOff;
  /* Infinite loop */
  for(;;)
  {
	  pOn = pwm[0]*2;
	  pOff = 200-pOn;
	  HAL_GPIO_WritePin(ledPort[0],ledGPIO[0],1);
      uDelay(pOn);
	  HAL_GPIO_WritePin(ledPort[0],ledGPIO[0],0);
      uDelay(pOff);
  }
  /* USER CODE END pwm1Thread */
}

/* USER CODE BEGIN Header_pwm2Thread */
/**
* @brief Function implementing the pwm2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm2Thread */
void pwm2Thread(void *argument)
{
  /* USER CODE BEGIN pwm2Thread */
	int pOn,pOff;
  /* Infinite loop */
  for(;;)
  {
	  pOn = pwm[1]*2;
	  pOff = 200-pOn;
	  HAL_GPIO_WritePin(ledPort[1],ledGPIO[1],1);
      uDelay(pOn);
	  HAL_GPIO_WritePin(ledPort[1],ledGPIO[1],0);
      uDelay(pOff);
  }
  /* USER CODE END pwm2Thread */
}

/* USER CODE BEGIN Header_pwm3Thread */
/**
* @brief Function implementing the pwm3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm3Thread */
void pwm3Thread(void *argument)
{
  /* USER CODE BEGIN pwm3Thread */
	int pOn,pOff;
  /* Infinite loop */
  for(;;)
  {
	  pOn = pwm[2]*2;
	  pOff = 200-pOn;
	  HAL_GPIO_WritePin(ledPort[2],ledGPIO[2],1);
      uDelay(pOn);
	  HAL_GPIO_WritePin(ledPort[2],ledGPIO[2],0);
      uDelay(pOff);
  }
  /* USER CODE END pwm3Thread */
}

/* USER CODE BEGIN Header_pwm4Thread */
/**
* @brief Function implementing the pwm4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm4Thread */
void pwm4Thread(void *argument)
{
  /* USER CODE BEGIN pwm4Thread */
	int pOn,pOff;
  /* Infinite loop */
  for(;;)
  {
	  pOn = pwm[3]*2;
	  pOff = 200-pOn;
	  HAL_GPIO_WritePin(ledPort[3],ledGPIO[3],1);
      uDelay(pOn);
	  HAL_GPIO_WritePin(ledPort[3],ledGPIO[3],0);
      uDelay(pOff);
  }
  /* USER CODE END pwm4Thread */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
