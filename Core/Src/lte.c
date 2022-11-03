#include "lte.h"

uint8_t LTE_Rx_Buffer[100];

void LTE_Init(void)
{
    if (HAL_UART_Receive_DMA(&huart1, LTE_Rx_Buffer, 100) != HAL_OK)
    {
      Error_Handler();
    }
}