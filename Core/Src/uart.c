#include "uart.h"
#include "lidar.h"

uint32_t uart1_counter,uart2_counter,uart3_counter;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        uart1_counter++;
        //HAL_UART_Transmit(&huart3,&huart->Instance->RDR,1,10);
    }
    else if(huart->Instance == USART2)
    {
        uart2_counter++;
        //HAL_UART_Transmit(&huart3,&huart->Instance->RDR,1,10);
    }
    else if(huart->Instance == USART3)
    {
        uart3_counter++;
    }
}