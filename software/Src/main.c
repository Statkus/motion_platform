/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "motion_platform_configuration.h"
#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct
{
  GPIO_TypeDef *Port;
  uint16_t Pin;
} GPIO;

typedef struct
{
  GPIO Pulse_GPIO;
  GPIO Direction_GPIO;
  GPIO Ready_GPIO;
  GPIO Torque_Alarm_GPIO;

  int32_t Position;
  uint8_t Position_Zero_Centered;
  int32_t Min_Position;
  int32_t Max_Position;
  int32_t Target_Position;
  GPIO_PinState Direction;

  uint8_t Needs_Calibration;
  uint8_t Min_Position_Calibrated;
  uint8_t Max_Position_Calibrated;
} Motor;

typedef enum
{
  Initialization,
  Calibrate_Min_Position,
  Calibrate_Max_Position,
  Run
} State;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

Motor motors[NB_MOTORS] = {
  {.Pulse_GPIO =
    {.Port = M1_PUL_GPIO_Port,
     .Pin  = M1_PUL_Pin},
   .Direction_GPIO =
    {.Port = M1_DIR_GPIO_Port,
     .Pin  = M1_DIR_Pin},
   .Ready_GPIO =
    {.Port = M1_READY_GPIO_Port,
     .Pin  = M1_READY_Pin},
   .Torque_Alarm_GPIO =
    {.Port = M1_TORQUE_ALARM_GPIO_Port,
     .Pin  = M1_TORQUE_ALARM_Pin},

   .Position               = 0,
   .Position_Zero_Centered = M1_POSITION_ZERO_CENTERED,
   .Min_Position           = M1_MIN_POSITION,
   .Max_Position           = M1_MAX_POSITION,
   .Target_Position        = 0,
   .Direction              = GPIO_PIN_RESET,

   .Needs_Calibration       = M1_NEEDS_CALIBRATION,
   .Min_Position_Calibrated = 0,
   .Max_Position_Calibrated = 0},
};
int32_t m_speed        = 1; // step/s
int32_t m_target_speed = 0; // step/s

// TODO: remove after correct value found and use define instead
//uint16_t M_MAX_SPEED        = 25000;
//uint16_t M_MAX_ACCELERATION = 100;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

void Receive_Commands(void);
void Move_Motors_To_Min_Position(void);
void Move_Motors_To_Max_Position(void);
void Move_Motors_To_Target(void);
void Delay_Us(uint16_t us);

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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  State state = Initialization;

  for (int i = 0; i < NB_MOTORS; i++) {
    motors[i].Direction = GPIO_PIN_RESET;

    HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(motors[i].Direction_GPIO.Port, motors[i].Direction_GPIO.Pin, motors[i].Direction);
  }

  HAL_TIM_Base_Start(&htim3);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_SET)
    //if (HAL_GPIO_ReadPin(motors[0].Torque_Alarm_GPIO.Port, motors[0].Torque_Alarm_GPIO.Pin) == GPIO_PIN_RESET)
    //{
    //  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    //}
    //else
    //{
    //  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    //}

    switch (state)
    {
      case Initialization:
      {
        uint8_t motors_ready = 1;

        for (int i = 0; i < NB_MOTORS; i++)
        {
          if (HAL_GPIO_ReadPin(motors[i].Ready_GPIO.Port, motors[i].Ready_GPIO.Pin) == GPIO_PIN_SET)
          {
            motors_ready = 0;
          }
        }

        if (motors_ready > 0)
        {
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

          if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_SET)
          {
            state = Calibrate_Min_Position;
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
          }
        }
        else
        {
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        }

        break;
      }

      case Calibrate_Min_Position:
      {
        uint8_t needs_calibration = 0;

        for (int i = 0; i < NB_MOTORS; i++)
        {
          if (motors[i].Needs_Calibration > 0 && motors[i].Min_Position_Calibrated == 0)
          {
            if (HAL_GPIO_ReadPin(motors[i].Torque_Alarm_GPIO.Port, motors[i].Torque_Alarm_GPIO.Pin) == GPIO_PIN_RESET)
            {
              motors[i].Min_Position_Calibrated = 1;
              motors[i].Position                = 0;
              motors[i].Min_Position            = MIN_MAX_SAFETY_BUFFER;
              motors[i].Target_Position         = motors[i].Min_Position;
            }
            else
            {
              needs_calibration = 1;
            }
          }
        }

        if (needs_calibration > 0)
        {
          Move_Motors_To_Min_Position();
        }
        else
        {
          state = Calibrate_Max_Position;
        }

        break;
      }

      case Calibrate_Max_Position:
      {
        uint8_t needs_calibration = 0;

        for (int i = 0; i < NB_MOTORS; i++)
        {
          if (motors[i].Needs_Calibration > 0 && motors[i].Max_Position_Calibrated == 0)
          {
            if (HAL_GPIO_ReadPin(motors[i].Torque_Alarm_GPIO.Port, motors[i].Torque_Alarm_GPIO.Pin) == GPIO_PIN_RESET && motors[i].Position > motors[i].Min_Position)
            {
              motors[i].Max_Position_Calibrated = 1;
              motors[i].Max_Position            = motors[i].Position - MIN_MAX_SAFETY_BUFFER;

              if (motors[i].Position_Zero_Centered > 0)
              {
                // Center positions to 0
                motors[i].Min_Position    = (motors[i].Min_Position - motors[i].Max_Position) / 2;
                motors[i].Max_Position    = -motors[i].Min_Position;
                motors[i].Position        = motors[i].Max_Position + MIN_MAX_SAFETY_BUFFER;

                motors[i].Target_Position = 0;
              }
              else
              {
                motors[i].Target_Position = (motors[i].Max_Position - motors[i].Min_Position) / 2;
              }

              m_target_speed = M_MAX_SPEED / 2;
            }
            else
            {
              needs_calibration = 1;
            }
          }
        }

        if (needs_calibration > 0)
        {
          Move_Motors_To_Max_Position();
        }
        else
        {
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

          state = Run;
        }

        break;
      }

      case Run:
        Receive_Commands();

        Move_Motors_To_Target();

        break;
    }
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 239;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  htim4.Init.Prescaler = 9;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED_Pin|M5_PUL_Pin|M5_DIR_Pin|M6_PUL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, M1_PUL_Pin|M1_DIR_Pin|M2_PUL_Pin|M2_DIR_Pin
                          |M3_PUL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, M3_DIR_Pin|M4_PUL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, M4_DIR_Pin|M6_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : M1_PUL_Pin M1_DIR_Pin M2_PUL_Pin M2_DIR_Pin
                           M3_PUL_Pin */
  GPIO_InitStruct.Pin = M1_PUL_Pin|M1_DIR_Pin|M2_PUL_Pin|M2_DIR_Pin
                          |M3_PUL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : M1_READY_Pin M1_TORQUE_ALARM_Pin M2_READY_Pin */
  GPIO_InitStruct.Pin = M1_READY_Pin|M1_TORQUE_ALARM_Pin|M2_READY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : M3_DIR_Pin M4_PUL_Pin */
  GPIO_InitStruct.Pin = M3_DIR_Pin|M4_PUL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : M2_TORQUE_ALARM_Pin M3_READY_Pin M5_TORQUE_ALARM_Pin M6_READY_Pin
                           M6_TORQUE_ALARM_Pin */
  GPIO_InitStruct.Pin = M2_TORQUE_ALARM_Pin|M3_READY_Pin|M5_TORQUE_ALARM_Pin|M6_READY_Pin
                          |M6_TORQUE_ALARM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : M4_DIR_Pin M6_DIR_Pin */
  GPIO_InitStruct.Pin = M4_DIR_Pin|M6_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : M5_PUL_Pin M5_DIR_Pin M6_PUL_Pin */
  GPIO_InitStruct.Pin = M5_PUL_Pin|M5_DIR_Pin|M6_PUL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin M3_TORQUE_ALARM_Pin M4_READY_Pin M4_TORQUE_ALARM_Pin
                           M5_READY_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|M3_TORQUE_ALARM_Pin|M4_READY_Pin|M4_TORQUE_ALARM_Pin
                          |M5_READY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_FLASH_CS_Pin */
  GPIO_InitStruct.Pin = SPI_FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI_FLASH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_CLOSE);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void Receive_Commands(void)
{
  if (Is_New_Data() > 0)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    int32_t pos_target = 0;
    int32_t dist       = 0;
    int32_t max_dist   = 0;

    for (int i = 0; i < NB_MOTORS; i++)
    {
      pos_target = Get_M_Pos_Target(i);

      if (motors[i].Position_Zero_Centered > 0)
      {
        pos_target -= 32767;
      }

      motors[i].Target_Position =
       MIN(MAX(pos_target, motors[i].Min_Position), motors[i].Max_Position);

      dist = motors[i].Target_Position - motors[i].Position;

      if (abs(dist) > abs(max_dist))
      {
        max_dist = dist;
      }
    }

    //M_MAX_SPEED        = (uint16_t)(Get_Max_Speed()) * 100;
    //M_MAX_ACCELERATION = (uint16_t)(Get_Max_Acceleration()) * 100;

    m_target_speed = MAX(MIN((max_dist * 1000000) / MESSAGE_PERIOD, M_MAX_SPEED), -M_MAX_SPEED);

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 500 + (Get_Shaker_PWM() * 2));
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, Get_Fan_PWM() * 4);
  }
}

void Move_Motors_To_Min_Position(void)
{
  uint8_t change_direction = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Needs_Calibration > 0 && motors[i].Min_Position_Calibrated == 0)
    {
      if (motors[i].Direction != GPIO_PIN_SET)
      {
        motors[i].Direction = GPIO_PIN_SET;

        HAL_GPIO_WritePin(motors[i].Direction_GPIO.Port, motors[i].Direction_GPIO.Pin, motors[i].Direction);

        change_direction = 1;
      }
    }
  }

  if (change_direction > 0)
  {
    Delay_Us(DIRECTION_DELAY_US);
  }

  uint8_t change_position = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Needs_Calibration > 0 && motors[i].Min_Position_Calibrated == 0)
    {
      change_position = 1;

      HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_RESET);
    }
  }

  if (change_position > 0)
  {
    Delay_Us(PULSE_DOWN_DELAY_US);

    for (int i = 0; i < NB_MOTORS; i++)
    {
      if (motors[i].Needs_Calibration > 0 && motors[i].Min_Position_Calibrated == 0)
      {
        HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_SET);
      }
    }

    Delay_Us(CALIBRATION_STEP_DELAY_US);
  }
}

void Move_Motors_To_Max_Position(void)
{
  uint8_t change_direction = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Needs_Calibration > 0 && motors[i].Max_Position_Calibrated == 0)
    {
      if (motors[i].Direction != GPIO_PIN_RESET)
      {
        motors[i].Direction = GPIO_PIN_RESET;

        HAL_GPIO_WritePin(motors[i].Direction_GPIO.Port, motors[i].Direction_GPIO.Pin, motors[i].Direction);

        change_direction = 1;
      }
    }
  }

  if (change_direction > 0)
  {
    Delay_Us(DIRECTION_DELAY_US);
  }

  uint8_t change_position = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Needs_Calibration > 0 && motors[i].Max_Position_Calibrated == 0)
    {
      change_position = 1;

      HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_RESET);
    }
  }

  if (change_position > 0)
  {
    Delay_Us(PULSE_DOWN_DELAY_US);

    for (int i = 0; i < NB_MOTORS; i++)
    {
      if (motors[i].Needs_Calibration > 0 && motors[i].Max_Position_Calibrated == 0)
      {
        if (motors[i].Direction == GPIO_PIN_RESET)
        {
          motors[i].Position++;
        }
        else
        {
          motors[i].Position--;
        }

        HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_SET);
      }
    }

    Delay_Us(CALIBRATION_STEP_DELAY_US);
  }
}

void Move_Motors_To_Target(void)
{
  uint8_t change_direction = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Position != motors[i].Target_Position)
    {
      const GPIO_PinState new_direction =
       (motors[i].Position < motors[i].Target_Position) ? GPIO_PIN_RESET : GPIO_PIN_SET;

      if (new_direction != motors[i].Direction)
      {
        motors[i].Direction = new_direction;

        HAL_GPIO_WritePin(motors[i].Direction_GPIO.Port, motors[i].Direction_GPIO.Pin, motors[i].Direction);

        change_direction = 1;
      }
    }
  }

  if (change_direction > 0)
  {
    Delay_Us(DIRECTION_DELAY_US);

    m_speed = 0;
  }

  uint8_t change_position = 0;

  for (int i = 0; i < NB_MOTORS; i++)
  {
    if (motors[i].Position != motors[i].Target_Position)
    {
      change_position = 1;

      HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_RESET);
    }
  }

  if (change_position > 0)
  {
    Delay_Us(PULSE_DOWN_DELAY_US);

    for (int i = 0; i < NB_MOTORS; i++)
    {
      if (motors[i].Position != motors[i].Target_Position)
      {
        if (motors[i].Direction == GPIO_PIN_RESET)
        {
          motors[i].Position++;
        }
        else
        {
          motors[i].Position--;
        }

        HAL_GPIO_WritePin(motors[i].Pulse_GPIO.Port, motors[i].Pulse_GPIO.Pin, GPIO_PIN_SET);
      }
    }

    if (m_speed != m_target_speed)
    {
      int32_t error = m_target_speed - m_speed;

      if (abs(error) <= M_MAX_ACCELERATION)
      {
        m_speed = m_target_speed;
      }
      else
      {
        if (error > 0)
        {
          m_speed += M_MAX_ACCELERATION;
        }
        else
        {
          m_speed -= M_MAX_ACCELERATION;
        }
      }
    }

    if (m_speed != 0)
    {
      Delay_Us((uint16_t)(MIN(1000000 / abs(m_speed), MAX_SPEED_DELAY_US)));
    }
    else
    {
      Delay_Us(MAX_SPEED_DELAY_US);
    }

  }
  else
  {
    m_speed = 0;
  }
}

void Delay_Us (uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim3, 0);
  while (__HAL_TIM_GET_COUNTER(&htim3) < us);
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
