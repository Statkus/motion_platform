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
#define M2_PUL_Pin GPIO_PIN_4
#define M2_PUL_GPIO_Port GPIOA
#define M2_DIR_Pin GPIO_PIN_5
#define M2_DIR_GPIO_Port GPIOA
#define M2_READY_Pin GPIO_PIN_6
#define M2_READY_GPIO_Port GPIOA
#define M3_PUL_Pin GPIO_PIN_7
#define M3_PUL_GPIO_Port GPIOA
#define M3_DIR_Pin GPIO_PIN_4
#define M3_DIR_GPIO_Port GPIOC
#define M4_PUL_Pin GPIO_PIN_5
#define M4_PUL_GPIO_Port GPIOC
#define M2_TORQUE_ALARM_Pin GPIO_PIN_0
#define M2_TORQUE_ALARM_GPIO_Port GPIOB
#define M4_DIR_Pin GPIO_PIN_1
#define M4_DIR_GPIO_Port GPIOB
#define M3_READY_Pin GPIO_PIN_2
#define M3_READY_GPIO_Port GPIOB
#define M5_PUL_Pin GPIO_PIN_7
#define M5_PUL_GPIO_Port GPIOE
#define M5_DIR_Pin GPIO_PIN_8
#define M5_DIR_GPIO_Port GPIOE
#define M6_PUL_Pin GPIO_PIN_9
#define M6_PUL_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define M3_TORQUE_ALARM_Pin GPIO_PIN_12
#define M3_TORQUE_ALARM_GPIO_Port GPIOE
#define M4_READY_Pin GPIO_PIN_13
#define M4_READY_GPIO_Port GPIOE
#define M4_TORQUE_ALARM_Pin GPIO_PIN_14
#define M4_TORQUE_ALARM_GPIO_Port GPIOE
#define M5_READY_Pin GPIO_PIN_15
#define M5_READY_GPIO_Port GPIOE
#define M5_TORQUE_ALARM_Pin GPIO_PIN_10
#define M5_TORQUE_ALARM_GPIO_Port GPIOB
#define M6_READY_Pin GPIO_PIN_11
#define M6_READY_GPIO_Port GPIOB
#define M6_TORQUE_ALARM_Pin GPIO_PIN_12
#define M6_TORQUE_ALARM_GPIO_Port GPIOB
#define M6_DIR_Pin GPIO_PIN_13
#define M6_DIR_GPIO_Port GPIOB
#define SHAKER_PWM_Pin GPIO_PIN_14
#define SHAKER_PWM_GPIO_Port GPIOB
#define FAN_PWM_Pin GPIO_PIN_15
#define FAN_PWM_GPIO_Port GPIOB
#define SPI_FLASH_CS_Pin GPIO_PIN_6
#define SPI_FLASH_CS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

#define DIRECTION_DELAY_US        5
#define PULSE_DOWN_DELAY_US       5
#define CALIBRATION_STEP_DELAY_US 300
#define MAX_SPEED_DELAY_US        1000

#define MIN_MAX_SAFETY_BUFFER 1000 // step

#define M_MAX_SPEED        40000 // step/s     // 40000 pour un comportement comme avant
#define M_MAX_ACCELERATION 2000  // step/s^2

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
