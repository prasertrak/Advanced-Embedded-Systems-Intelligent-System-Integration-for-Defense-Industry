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
#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAXDISTANCE 3000  //Maximum distance 3000uS/2
#define MAXDELAY	5     //Maximum wait in ms
#define MAXDETECT   1000  //Distance as detected
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim14;

/* Definitions for ultrasonicTask */
osThreadId_t ultrasonicTaskHandle;
const osThreadAttr_t ultrasonicTask_attributes = {
  .name = "ultrasonicTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for motorTask */
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTask_attributes = {
  .name = "motorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for switchTask */
osThreadId_t switchTaskHandle;
const osThreadAttr_t switchTask_attributes = {
  .name = "switchTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for joystickTask */
osThreadId_t joystickTaskHandle;
const osThreadAttr_t joystickTask_attributes = {
  .name = "joystickTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for mainTask */
osThreadId_t mainTaskHandle;
const osThreadAttr_t mainTask_attributes = {
  .name = "mainTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
//LED1-4
GPIO_TypeDef *ledPort[4] = {GPIOB,GPIOC,GPIOC,GPIOB};
int ledGPIO[4] = {GPIO_PIN_2,GPIO_PIN_6,GPIO_PIN_8,GPIO_PIN_3};

//SWITCH1-4
GPIO_TypeDef *keyPort[4] = {GPIOB,GPIOB,GPIOB,GPIOB};
int keyGPIO[4] = {GPIO_PIN_15,GPIO_PIN_12,GPIO_PIN_13,GPIO_PIN_14};

//ULTRASONIC1-2
GPIO_TypeDef *UP_trig[2] = {GPIOC,GPIOC};
GPIO_TypeDef *UP_echo[2] = {GPIOD,GPIOC};
uint16_t trig[2] = {GPIO_PIN_12,GPIO_PIN_10};
uint16_t echo[2] = {GPIO_PIN_2,GPIO_PIN_11};

int direction=0;
int speed=0;
int motorSpeed[2] = {0};
int ultra[2] = {-1,-1};
int ultraEN = 0;
int servoEN = 0;
int joyX,joyY;
int servoPWM[2] = {50,50};
char msg[64];
int distance[19]={0};

uint32_t uStart1,uStart2,uEnd1,uEnd2;
uint32_t uTime[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM14_Init(void);
void ultrasonicThread(void *argument);
void motorThread(void *argument);
void switchThread(void *argument);
void joystickThread(void *argument);
void mainThread(void *argument);

/* USER CODE BEGIN PFP */
int getch(int sw);
void lightLED(int no,int value);
void clearTimer();
uint32_t getTimer();
void uDelay(uint32_t us);
int ultraSonic(int no);
void setADCChannel(ADC_HandleTypeDef *hadc, uint32_t chan);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_2){ //left side
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)){ //start positive pulse
			uStart1 = __HAL_TIM_GetCounter(&htim2); //Read timestamp
		}else{ //ending of positive pulse
			uEnd1 = __HAL_TIM_GetCounter(&htim2);
			uTime[0] = uEnd1-uStart1;
		}
	}else if(GPIO_Pin == GPIO_PIN_11){ //right side
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)){ //start positive pulse
			uStart2 = __HAL_TIM_GetCounter(&htim2); //Read timestamp
		}else{ //ending of positive pulse
			uEnd2 = __HAL_TIM_GetCounter(&htim2);
			uTime[1] = uEnd2-uStart2;
		}
	}
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
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  HAL_GPIO_WritePin(UP_trig[0],trig[0],0);
  HAL_GPIO_WritePin(UP_trig[1],trig[1],0);
  ssd1306_Init();
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
  /* creation of ultrasonicTask */
  ultrasonicTaskHandle = osThreadNew(ultrasonicThread, NULL, &ultrasonicTask_attributes);

  /* creation of motorTask */
  motorTaskHandle = osThreadNew(motorThread, NULL, &motorTask_attributes);

  /* creation of switchTask */
  switchTaskHandle = osThreadNew(switchThread, NULL, &switchTask_attributes);

  /* creation of joystickTask */
  joystickTaskHandle = osThreadNew(joystickThread, NULL, &joystickTask_attributes);

  /* creation of mainTask */
  mainTaskHandle = osThreadNew(mainThread, NULL, &mainTask_attributes);

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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  htim2.Init.Prescaler = 90-1;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1800-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1800-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 180-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 10000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 180-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 10000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED2_Pin|LED3_Pin|TRIG1_Pin|TRIG2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED4_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SW2_Pin SW3_Pin SW4_Pin SW1_Pin */
  GPIO_InitStruct.Pin = SW2_Pin|SW3_Pin|SW4_Pin|SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED3_Pin TRIG1_Pin TRIG2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED3_Pin|TRIG1_Pin|TRIG2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ECHO1_Pin */
  GPIO_InitStruct.Pin = ECHO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ECHO2_Pin */
  GPIO_InitStruct.Pin = ECHO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int getch(int sw){
	if(HAL_GPIO_ReadPin(keyPort[sw],keyGPIO[sw]))
		return 0; //not pressed
	HAL_Delay(20);
	//Wait for key unpressed
	while(!HAL_GPIO_ReadPin(keyPort[sw],keyGPIO[sw]))
		HAL_Delay(20);
	return 1;
}

void lightLED(int no,int value){
	HAL_GPIO_WritePin(ledPort[no],ledGPIO[no],value);
}

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

int ultraSonic(int no){
	clearTimer();
	HAL_GPIO_WritePin(UP_trig[no],trig[no],1);
	uDelay(10);
	HAL_GPIO_WritePin(UP_trig[no],trig[no],0);
    uTime[no]=-1;
	//wait for 4ms to allow  ECHO response to capture
	//using EXTI interrupt
    HAL_Delay(MAXDELAY);
    if((uTime[no]<1)||(uTime[no]>MAXDISTANCE))
		return -1;
    return uTime[no];
}

void setADCChannel(ADC_HandleTypeDef *hadc, uint32_t chan){
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = chan;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		Error_Handler();
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_ultrasonicThread */
/**
  * @brief  Function implementing the ultrasonicTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ultrasonicThread */
void ultrasonicThread(void *argument)
{
  /* USER CODE BEGIN 5 */
	int r,i,tmp;
	int step[18]={0,1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1};
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
	TIM8->CCR4 = 500 + servoPWM[0]*5;
	HAL_TIM_PWM_Start(&htim14,TIM_CHANNEL_1);
	TIM14->CCR1 = 500 + servoPWM[1]*5;
  /* Infinite loop */
  for(;;)
  {
	  if(servoEN){
		  //Servo movement mode
		  for(r=0;r<18;r++){
			 servoPWM[0]=step[r]*11;
			 servoPWM[1]=step[r]*11;
			 TIM8->CCR4 = 500 + servoPWM[0]*5;
			 TIM14->CCR1 = 500 + servoPWM[1]*5;
			 for(i=0;i<2;i++){
			 	tmp = ultraSonic(i);
			 	if(tmp<0) tmp = MAXDISTANCE;
			 	if(tmp>MAXDISTANCE) tmp=MAXDISTANCE;
			 	distance[i*9+step[r]]=tmp;
				osDelay(20);
			 }
			 ultra[0] = (distance[4]+distance[5]+distance[6]+distance[7]+distance[8])/5;
		     ultra[1] = (distance[10]+distance[11]+distance[12]+distance[13]+distance[14])/5;
			 osDelay(30);
		  }
	  }else{
		  //Fixed movement mode
		  servoPWM[0]=0; //Left servo
		  servoPWM[1]=100; //Right servo
		  TIM8->CCR4 = 500 + servoPWM[0]*5;
		  TIM14->CCR1 = 500 + servoPWM[1]*5;

		  for(i=0;i<2;i++){
			  tmp = ultraSonic(i);
			  if(tmp>0) ultra[i]=tmp; else ultra[i] = MAXDISTANCE;
			  if(ultra[i] > MAXDISTANCE) ultra[i] = MAXDISTANCE;
			    osDelay(10);
		  }

		  for(r=0;r<10;r++){
			  distance[r]=ultra[0];
			  distance[r+9]=ultra[1];
		  }
		  distance[9]= (ultra[0]+ultra[1])/2;
		  osDelay(30);
	  }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_motorThread */
/**
* @brief Function implementing the motorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motorThread */
void motorThread(void *argument)
{
  /* USER CODE BEGIN motorThread */
	//Start motor PWM
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM4->CCR1 = 0;
	TIM4->CCR2 = 0;
  /* Infinite loop */
  for(;;)
  {
	  //Translate speed to each motor speed
	motorSpeed[0] = speed+direction;
	motorSpeed[1] = speed-direction;
	//Correcting out-of-bound speed
	if(motorSpeed[0]> 100)motorSpeed[0]= 100;
	if(motorSpeed[0]<-100)motorSpeed[0]=-100;
	if(motorSpeed[1]> 100)motorSpeed[1]= 100;
	if(motorSpeed[1]<-100)motorSpeed[1]=-100;

	if(motorSpeed[0]>=0){
		TIM4->CCR1 = motorSpeed[0]*10;
		TIM4->CCR2 = 0;
	}else{
		TIM4->CCR1 = 0;
		TIM4->CCR2 = (-motorSpeed[0])*10;
	}
	if(motorSpeed[1]>=0){
		TIM3->CCR1 = motorSpeed[1]*10;
		TIM3->CCR2 = 0;
	}else{
		TIM3->CCR1 = 0;
		TIM3->CCR2 = (-motorSpeed[1])*10;
	}
	osDelay(50);
  }
  /* USER CODE END motorThread */
}

/* USER CODE BEGIN Header_switchThread */
/**
* @brief Function implementing the switchTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_switchThread */
void switchThread(void *argument)
{
  /* USER CODE BEGIN switchThread */
  int led2=1,led3=1;
  lightLED(2,ultraEN);
  lightLED(3,servoEN);
  int r,n=MAXDISTANCE/20;
  /* Infinite loop */
  for(;;)
  {
	  //Enable Ultrasonic avoidance mode
	  if(getch(0)) ultraEN = 0;
	  if(getch(1)) ultraEN = 1;
	  if(getch(2)) servoEN = 0;
	  if(getch(3)) servoEN = 1;
	if(ultraEN){
		led2 = !led2;
		lightLED(2,led2);
	}else lightLED(2,1);
	if(servoEN){
		led3 = !led3;
		lightLED(3,led3);
	}else lightLED(3,1);

	ssd1306_Fill(Black);
	ssd1306_SetCursor(0,0);
	if(ultraEN) ssd1306_WriteString("SEN: ON",Font_6x8,White);
	else ssd1306_WriteString("SEN: OFF",Font_6x8,White);

	ssd1306_SetCursor(50,0);
	if(servoEN) ssd1306_WriteString("MOV: ON",Font_6x8,White);
	else ssd1306_WriteString("MOV: OFF",Font_6x8,White);

	char ch;
	ch=' ';
	if(joyY < 120) ch='^';
	if(joyY > 135) ch='v';
	ssd1306_SetCursor(110,0);
	ssd1306_WriteChar(ch,Font_6x8,White);

	ch=' ';
	if(joyX < 120) ch='<';
	if(joyX > 135) ch='>';
	ssd1306_SetCursor(120,0);
	ssd1306_WriteChar(ch,Font_6x8,White);
	for(r=0;r<19;r++){
		int tmp = 31-distance[18-r]/n;
		if(tmp<0) tmp=0;
		if(tmp>31)tmp=31;
		ssd1306_DrawRectangle(r*5+10,tmp,r*5+14,31,White);
	}

	ssd1306_UpdateScreen();

    osDelay(200);
  }
  /* USER CODE END switchThread */
}

/* USER CODE BEGIN Header_joystickThread */
/**
* @brief Function implementing the joystickTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_joystickThread */
void joystickThread(void *argument)
{
  /* USER CODE BEGIN joystickThread */
  /* Infinite loop */
  for(;;)
  {
	  setADCChannel(&hadc1,ADC_CHANNEL_0);
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);
	  joyX=(unsigned)HAL_ADC_GetValue(&hadc1);
	  setADCChannel(&hadc1,ADC_CHANNEL_1);
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);
	  joyY=(unsigned)HAL_ADC_GetValue(&hadc1);
	  osDelay(100);

  }
  /* USER CODE END joystickThread */
}

/* USER CODE BEGIN Header_mainThread */
/**
* @brief Function implementing the mainTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_mainThread */
void mainThread(void *argument)
{
  /* USER CODE BEGIN mainThread */
	int tSpeed;
	int tDirection;
	int Ldetected,Rdetected;

  /* Infinite loop */
  for(;;)
  {
	// read joystick y-axis
	tSpeed=(128-joyY)/12;
	if(tSpeed>10)tSpeed=10;
	if(tSpeed<-10)tSpeed=-10;

	// read joystick x-axis
	tDirection=(joyX-128)/24;
	if(tDirection>5) tDirection=5;
	if(tDirection<-5) tDirection=-5;

	// read ultrasonics
	if((ultra[0]>0)&&(ultra[0]<MAXDETECT)&&ultraEN){
		Rdetected = 1;
		lightLED(0,1);
	}else{
		Rdetected = 0;
		lightLED(0,0);
	}
	if((ultra[1]>0)&&(ultra[1]<MAXDETECT)&&ultraEN){
		Ldetected = 1;
		lightLED(1,1);
	}else{
		Ldetected = 0;
		lightLED(1,0);
	}
	// both ultrasonic detected //stop (allow reverse)
	if(Ldetected&&Rdetected){
		if(tSpeed>=0) tSpeed=-2; //reverse
	}
	else if(Ldetected)  //back-turn to the right
		{tSpeed=-2; tDirection=3;}
	else if(Rdetected)  //back-turn to the left
	    {tSpeed=-2; tDirection=-3;}
	speed = tSpeed*10;
	direction = tDirection*10;
	osDelay(50);
  }
  /* USER CODE END mainThread */
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
