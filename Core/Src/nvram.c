#include "nvram.h"
#include "lte.h"
#include "device.h"

static HAL_StatusTypeDef EraseNV();

uint32_t NVRAM_Block[MAX_NVRAM_SIZE];
uint8_t NVRAM_Domain[100];
char debug_buf[100];

void NVRam_Init(void)
{
    if( *(__IO uint64_t*)(FLASH_USER_START_ADDR) == 0xFFFFFFFFFFFFFFFF)
    { 
        memset(NVRAM_Block, 0x00, MAX_NVRAM_SIZE * 4);
        NVRAM_Block[NVRAM_SN] = 1;
        NVRAM_Block[NVRAM_LATITUDE_I] = 0;
        NVRAM_Block[NVRAM_LATITUDE_D] = 0;
        NVRAM_Block[NVRAM_LONGITUDE_I] = 0;
        NVRAM_Block[NVRAM_LONGITUDE_D] = 0;
        NVRAM_Block[NVRAM_REPORT_CYCLE] = 3 * 60 * 60;
        
        for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
        {
            int db_index = NVRAM_DOMAIN + (i / 4);
            NVRAM_Block[db_index] = (NVRAM_Domain[i+3] << 24) + (NVRAM_Domain[i+2] << 16) + (NVRAM_Domain[i+1] << 8) + NVRAM_Domain[i];
        }
        
        NVRam_Write();
    }
    else 
    {
        NVRam_Read();
    }
    
    return;
}

void NVRam_Read(void)
{
    memset(NVRAM_Block, 0x00, MAX_NVRAM_SIZE * 4);
    for(int i = 0; i < MAX_NVRAM_SIZE; i++) NVRAM_Block[i] = *(__IO uint64_t*)(FLASH_USER_START_ADDR + (i * 4));  
    
    LTE_SN = NVRAM_Block[NVRAM_SN];
    LTE_SN_SUB = NVRAM_Block[NVRAM_SN_SUB];
    LTE_GPS_LAT_I = NVRAM_Block[NVRAM_LATITUDE_I];
    LTE_GPS_LAT_D = NVRAM_Block[NVRAM_LATITUDE_D];
    LTE_GPS_LONG_I = NVRAM_Block[NVRAM_LONGITUDE_I];
    LTE_GPS_LONG_D = NVRAM_Block[NVRAM_LONGITUDE_D];
    Device_Report_Cycle = NVRAM_Block[NVRAM_REPORT_CYCLE];
    
    for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
    {
        int db_index = NVRAM_DOMAIN + (i / 4);
        NVRAM_Domain[i+3] = (char)((NVRAM_Block[db_index] & 0xFF000000) >> 24);
        NVRAM_Domain[i+2] = (char)((NVRAM_Block[db_index] & 0x00FF0000) >> 16);
        NVRAM_Domain[i+1] = (char)((NVRAM_Block[db_index] & 0x0000FF00) >> 8);
        NVRAM_Domain[i] = (char)((NVRAM_Block[db_index] & 0x000000FF));
    }
}

void NVRam_Write(void)
{  
    HAL_FLASH_Unlock();
    HAL_Delay(10U);
    for(int i = 0; i < MAX_NVRAM_SIZE; i++)
    {
        __disable_irq();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR + (i * 4), NVRAM_Block[i]);
        __enable_irq();
        HAL_Delay(10U);
    }
    HAL_FLASH_Lock();  
    return;
}

HAL_StatusTypeDef EraseNV(void)
{  
    static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages     = 1;

	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		HAL_FLASH_Lock();
        HAL_Delay(10);
        NVIC_SystemReset();
	}
    HAL_FLASH_Lock();
    
    return HAL_OK;  
}

void NVRam_Update_Variable(void)
{
    EraseNV();
    HAL_Delay(100);
    NVRam_Write();
    HAL_Delay(500);
    NVIC_SystemReset();
}
