#include "dht11.h"

static void DHT11_ModeSelection(DHT_Mode_t mode);

void DHT11_Init(void)
{
    /* DHT11 Sensor ON */
	HAL_GPIO_WritePin(TEMP_ONOFF_GPIO_Port, TEMP_ONOFF_Pin, GPIO_PIN_SET);
}

DHT_Data_t DHT_getData(DHT_type_t type) 
{
	DHT_Data_t data = {0.0f,};
    static DHT_Data_t stored_data;
    static uint32_t lastPollingTime;
    static uint8_t bFirstcall;
	uint32_t timeout = 0;
	uint16_t pollingInterval;

	if (type == DHT11) 
    {
		pollingInterval = DHT_POLLING_INTERVAL_DHT11;
	} 
    else 
    {
		pollingInterval = DHT_POLLING_INTERVAL_DHT22;
	}
    if(bFirstcall == 1)
    {
        if ((HAL_GetTick()-lastPollingTime) < pollingInterval) 
        {
            data.Temperature = stored_data.Temperature;
            data.Humidity = stored_data.Humidity;
            return data;
        }
    }
    if(bFirstcall == 0)
    {
        bFirstcall = 1; 
    }

	lastPollingTime = HAL_GetTick();

    /* Read Sensor Data*/
    DHT11_ModeSelection(OUTPUT_MODE);
    HAL_GPIO_WritePin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
    HAL_GPIO_WritePin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin, GPIO_PIN_SET);
    DHT11_ModeSelection(INPUT_MODE);

	while(HAL_GPIO_ReadPin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin) == GPIO_PIN_SET) 
    {
		timeout++;
		if (timeout > DHT_TIMEOUT)
        {
            return data;
        } 
	}
  
	timeout = 0;
	while(!(HAL_GPIO_ReadPin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin) == GPIO_PIN_SET)) 
    {
		timeout++;
		if (timeout > DHT_TIMEOUT)
        {
            return data;
        }
	}
  
	timeout = 0;
	while(HAL_GPIO_ReadPin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin) == GPIO_PIN_SET) 
    {
		timeout++;
		if (timeout > DHT_TIMEOUT) 
        {
            return data;
        }
	}

 	uint8_t rawData[5] = {0,0,0,0,0};
	for(uint8_t a = 0; a < 5; a++) 
    {
		for(uint8_t b = 7; b != 255; b--) 
        {
			uint32_t hT = 0, lT = 0;
            timeout = 0;
			while(!(HAL_GPIO_ReadPin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin) == GPIO_PIN_SET))
            {
                lT++;
                timeout++;
		        if (timeout > DHT_TIMEOUT2) 
                {
                    break;
                }
            }
			timeout = 0;
			while(HAL_GPIO_ReadPin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin) == GPIO_PIN_SET)
            {
                hT++;
                timeout++;
		        if (timeout > DHT_TIMEOUT2) 
                {
                    break;
                }
            }
			if(hT > lT) 
            {
                rawData[a] |= (1<<b);
            }
		}
	}
 
	if((uint8_t)(rawData[0] + rawData[1] + rawData[2] + rawData[3]) == rawData[4]) 
    {
		if (type == DHT22) 
        {
			data.Humidity = (float)(((uint16_t)rawData[0]<<8) | rawData[1])*0.1f;

			if(!(rawData[2] & (1<<7))) 
            {
				data.Temperature = (float)(((uint16_t)rawData[2]<<8) | rawData[3])*0.1f;
			}	
            else 
            {
				rawData[2] &= ~(1<<7);
				data.Temperature = (float)(((uint16_t)rawData[2]<<8) | rawData[3])*-0.1f;
			}
		}
		if (type == DHT11) 
        {
			data.Humidity = (float)rawData[0];
			data.Temperature = (float)rawData[2];;
		}
	}
	
	stored_data.Humidity = data.Humidity;
	stored_data.Temperature = data.Temperature;

	return data;	
}

static void DHT11_ModeSelection(DHT_Mode_t mode) 
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(mode == OUTPUT_MODE)
    {
        HAL_GPIO_WritePin(DHT_INOUT_GPIO_Port, DHT_INOUT_Pin, GPIO_PIN_SET);
        GPIO_InitStruct.Pin = DHT_INOUT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; 	
        GPIO_InitStruct.Pull = GPIO_NOPULL;		
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT_INOUT_GPIO_Port, &GPIO_InitStruct);
    }
    else
    {
        GPIO_InitStruct.Pin = DHT_INOUT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;				
        HAL_GPIO_Init(DHT_INOUT_GPIO_Port, &GPIO_InitStruct);
    }
}