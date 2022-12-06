#include "device.h"

uint32_t Device_Batt_DMA;
uint16_t Device_Batt_Voltage;
uint8_t Device_Sensor_Signal;
uint8_t Device_Sensor_Detect;

Reset_Cause_t reset_cause = RESET_CAUSE_UNKNOWN;

void Device_Init(void)
{
	// Battery ADC
	if (HAL_ADC_Start_DMA(&DEVICE_BATTERY_MODULE, &Device_Batt_DMA, 1) != HAL_OK)
	{
		Error_Handler();
	}
	 
}

void Device_Reset_Init(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reset_cause = RESET_CAUSE_LOW_POWER_RESET;
    } 
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        reset_cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    } 
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        reset_cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;  
    }
    
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        reset_cause = RESET_CAUSE_SOFTWARE_RESET; // This reset is induced by calling the ARM CMSIS `NVIC_SystemReset()` function!
    }  
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }  
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        reset_cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }  
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY))
    {
        reset_cause = RESET_CAUSE_LS_OSC_READY;
    }  
	
    if (reset_cause == RESET_CAUSE_POWER_ON_POWER_DOWN_RESET)
    {
        NVIC_SystemReset();
    }
    if (reset_cause == RESET_CAUSE_EXTERNAL_RESET_PIN_RESET)
    {
        NVIC_SystemReset();
    }
    if (reset_cause == RESET_CAUSE_LS_OSC_READY)
    {
        NVIC_SystemReset();
    }
}

void Device_Battery_Voltage_Read(void)
{
    float AdcVoltLoc = 0.0f;
	// Battery ADC
	if (HAL_ADC_Start_DMA(&DEVICE_BATTERY_MODULE, &Device_Batt_DMA, 1) != HAL_OK)
	{
		Error_Handler();
	}    
    /* Convert ADC Value To MCU Sensing Voltage*/
    /* ADC Has 12-bit Resolution(2^12 = 4096) */
    AdcVoltLoc = ((float)Device_Batt_DMA * 3300.0f / 4096.0f);
    /* Convert MCU Sensing Voltage To Actual Input Voltage */
    Device_Batt_Voltage = (uint16_t)(AdcVoltLoc * (22000.0f + 10000.0f) / (10000.0f));
}

void Device_Detect_SensorSignal(void)
{
    static uint8_t ti10msOn;
    static uint8_t ti10msOff;
    uint8_t stgpio;

    stgpio = HAL_GPIO_ReadPin(DOOR_INT_GPIO_Port,DOOR_INT_Pin);
    if(GPIO_PIN_SET == stgpio)
    {
        Device_Sensor_Detect = DEVICE_SENSOR_DETECTED;
    }
    else
    {
        Device_Sensor_Detect = 0;            
        Device_Sensor_Signal = DEVICE_SENSOR_OFF;
    }

    if(DEVICE_SENSOR_DETECTED == Device_Sensor_Detect)
    {
        ti10msOn++;
        if(ti10msOn>= DEVICE_SENSOR_ON_DEBOUNCE_TIME)
        {
            ti10msOn = 0;
            Device_Sensor_Signal = DEVICE_SENSOR_ON;
        }
    }
    else
    {
        ti10msOn = 0;
    }
}
