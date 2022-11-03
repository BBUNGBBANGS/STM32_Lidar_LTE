#include "uart.h"
#include "lte.h"
#include "debug.h"
uint8_t dummy;
uint32_t uart1_counter,uart2_counter,uart3_counter;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        LTE_Rx_Buffer[LTE_Rx_Counter] = huart->Instance->RDR;
        LTE_Rx_Counter++;
        HAL_UART_Receive_IT(&huart1,&dummy,1);
    }
    else if(huart->Instance == USART2)
    {
        uart2_counter++;
    }
    else if(huart->Instance == USART3)
    {
        Debug_Rx_buffer[Debug_Rx_Count] = huart->Instance->RDR;
        Debug_Rx_Count++;
        HAL_UART_Receive_IT(&huart3,&dummy,1);
    }
}