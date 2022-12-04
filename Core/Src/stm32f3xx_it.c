/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    while (1)
    {
        HAL_NVIC_SystemReset();
    }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    while (1)
    {
        HAL_NVIC_SystemReset();
    }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    while (1)
    {
        HAL_NVIC_SystemReset();
    }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    while (1)
    {
        HAL_NVIC_SystemReset();
    }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    while (1)
    {
        HAL_NVIC_SystemReset();
    }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
    HAL_NVIC_SystemReset();
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
    HAL_NVIC_SystemReset();
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
    HAL_NVIC_SystemReset();
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/**
  * @brief This function handles ADC1 and ADC2 interrupts.
  */
void ADC1_2_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc2);
}


/**
  * @brief This function handles TIM1 update and TIM16 interrupts.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  	HAL_TIM_IRQHandler(&htim16);

}

/**
  * @brief This function handles TIM1 trigger, commutation and TIM17 interrupts.
  */
void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
  	HAL_TIM_IRQHandler(&htim17);

}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt / USART3 wake-up interrupt through EXTI line 28.
  */
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart3);
}


/**
  * @brief This function handles Timer 6 interrupt and DAC underrun interrupts.
  */
void DMA2_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc2);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
