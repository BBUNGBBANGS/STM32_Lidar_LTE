#include "lidar.h"

uint8_t Lidar_Rx_Buffer[9];
uint16_t Lidar_Distance;
uint16_t Lidar_Strength;
uint16_t Lidar_Temperature;

static uint8_t Lidar_Checksum(void);

void Lidar_Init(void)
{
    // UART2 -LIDAR
	if (HAL_UART_Receive_DMA(&LIDAR_MODULE, (uint8_t *)Lidar_Rx_Buffer, 9) != HAL_OK)
    {
		Error_Handler();
	}
    
	// Liadar  ON
	HAL_GPIO_WritePin(Lidar_ONOFF_GPIO_Port, Lidar_ONOFF_Pin, GPIO_PIN_SET);
}

void Lidar_Receive_Data(void)
{
    if(LIDAR_CHECKSUM_OK == Lidar_Checksum())
    {
        Lidar_Distance = (Lidar_Rx_Buffer[2] | (Lidar_Rx_Buffer[3] << 8))&(0xFFFF);
        Lidar_Strength = (Lidar_Rx_Buffer[4] | (Lidar_Rx_Buffer[5] << 8))&(0xFFFF);
        Lidar_Temperature = (Lidar_Rx_Buffer[6] | (Lidar_Rx_Buffer[7] << 8))&(0xFFFF);
    }
}

static uint8_t Lidar_Checksum(void)
{
    uint16_t checksum = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        checksum += Lidar_Rx_Buffer[i];
    }

    if(Lidar_Rx_Buffer[8] == (checksum&0xFF))
    {
        return LIDAR_CHECKSUM_OK;
    }
    else
    {
        return LIDAR_CHECKSUM_FAIL;
    }
}