#include "lte.h"

uint8_t LTE_Rx_Buffer[1024];
uint16_t LTE_Rx_Counter;
uint8_t dummy_lte;
void LTE_Init(void)
{
    if ( HAL_UART_Receive_IT(&huart1, &dummy_lte, 1) != HAL_OK)
    {
        Error_Handler();
    }
    
	//SARA Power On
	HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    HAL_Delay(2000);
	//SARA Reset Off
	HAL_GPIO_WritePin(RESET_ONOFF_GPIO_Port, RESET_ONOFF_Pin, GPIO_PIN_RESET);
}