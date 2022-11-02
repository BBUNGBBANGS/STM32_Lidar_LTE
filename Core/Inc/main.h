/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define LTE_STATUS_Pin GPIO_PIN_1
#define LTE_STATUS_GPIO_Port GPIOA
#define Lidar_ONOFF_Pin GPIO_PIN_4
#define Lidar_ONOFF_GPIO_Port GPIOA
#define DOOR_INT_Pin GPIO_PIN_5
#define DOOR_INT_GPIO_Port GPIOA
#define DOOR_INT_EXTI_IRQn EXTI9_5_IRQn
#define BAT_ADC_Pin GPIO_PIN_6
#define BAT_ADC_GPIO_Port GPIOA
#define DHT_INOUT_Pin GPIO_PIN_7
#define DHT_INOUT_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOB
#define TEMP_ONOFF_Pin GPIO_PIN_1
#define TEMP_ONOFF_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define POWER_KEY_Pin GPIO_PIN_5
#define POWER_KEY_GPIO_Port GPIOB
#define RESET_ONOFF_Pin GPIO_PIN_6
#define RESET_ONOFF_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
