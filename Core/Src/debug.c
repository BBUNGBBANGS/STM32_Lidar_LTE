#include "debug.h"
#include "string.h"
#include "uart.h"

uint8_t Debug_Tx_buffer[100];

void Debug_Init(void)
{
    if (HAL_UART_Receive_DMA(&DEBUG_MODULE, Debug_Tx_buffer, UART_RX_BUFFER_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
    Debug_Message_Transmit("\n=======>>Silo Test F/W 20220628_002\n");
}

void Debug_Message_Transmit(uint8_t * buf)
{
    HAL_UART_Transmit(&DEBUG_MODULE,buf,strlen(buf),100);
}