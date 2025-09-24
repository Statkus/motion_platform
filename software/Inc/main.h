/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOE
#define BUTTON_Pin GPIO_PIN_13
#define BUTTON_GPIO_Port GPIOC
#define M1_PUL_Pin GPIO_PIN_0
#define M1_PUL_GPIO_Port GPIOA
#define M1_DIR_Pin GPIO_PIN_1
#define M1_DIR_GPIO_Port GPIOA
#define M1_READY_Pin GPIO_PIN_2
#define M1_READY_GPIO_Port GPIOA
#define M1_TORQUE_ALARM_Pin GPIO_PIN_3
#define M1_TORQUE_ALARM_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define SHAKER_PWM_Pin GPIO_PIN_12
#define SHAKER_PWM_GPIO_Port GPIOD
#define FAN_PWM_Pin GPIO_PIN_13
#define FAN_PWM_GPIO_Port GPIOD
#define SPI_FLASH_CS_Pin GPIO_PIN_6
#define SPI_FLASH_CS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

#define DIRECTION_DELAY  2
#define PULSE_DOWN_DELAY 2
#define HOME_STEP_DELAY  100

//#define M_MAX_SPEED        25000 // step/s
//#define M_MAX_ACCELERATION 5000

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
