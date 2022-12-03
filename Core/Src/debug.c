#include "debug.h"
#include "uart.h"
#include "lte.h"

uint8_t Debug_Rx_buffer[1024];
uint16_t Debug_Rx_Count;
uint8_t debug_dummy;
void Debug_Init(void)
{
    if ( HAL_UART_Receive_IT(&DEBUG_MODULE, &debug_dummy, 1) != HAL_OK)
	{
		Error_Handler();
	}
    Debug_Message_Transmit("\n=======>>Silo Test F/W 20220628_002\n");
}

void Debug_Message_Transmit(uint8_t * buf)
{
    HAL_UART_Transmit(&DEBUG_MODULE,buf,strlen(buf),100);
}

void Test_Debug(void)
{
    if(Debug_Rx_Count>0)
    {
        HAL_UART_Transmit(&huart3,Debug_Rx_buffer,Debug_Rx_Count,100);
        HAL_UART_Transmit(&huart1,Debug_Rx_buffer,Debug_Rx_Count,100);
        Debug_Rx_Count = 0;
    }
    #if 0
    if(LTE_Rx_Counter>0)
    {
        HAL_UART_Transmit(&huart3,LTE_Rx_Buffer,LTE_Rx_Counter,100);
        LTE_Rx_Counter = 0;
    }
    #endif
}