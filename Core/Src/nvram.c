#include "nvram.h"

#define MQTT_SERVER_PORT_NUM     1883
#define MAX_REPORT_TIME         21600  // 6 hr

void NV_Read(void);
void NV_Write(void);
HAL_StatusTypeDef EraseNV();
HAL_StatusTypeDef r_Val;

uint32_t nv_db[MAX_NVRAM_SIZE];
uint32_t longitude, latitude, report_cycle;
uint32_t longitude2 = 0, latitude2 = 0;
uint32_t gps_status = 0;
char mqtt_domain_name[100];
char debug_buf[100];
uint32_t mqtt_did_num;
uint32_t did_sub_num;

void NVRam_Init(void)
{
    if( *(__IO uint64_t*)(FLASH_USER_START_ADDR) == 0xFFFFFFFFFFFFFFFF)
    { 
        memset(nv_db, 0x00, MAX_NVRAM_SIZE * 4);
        nv_db[NVRAM_SN] = 1;
        nv_db[NVRAM_LONGITUDE] = 0;
        nv_db[NVRAM_LATITUDE] = 0;
        nv_db[NVRAM_REPORT_CYCLE] = 3 * 60 * 60;
        
        for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
        {
            int db_index = NVRAM_DOMAIN + (i / 4);
            nv_db[db_index] = (mqtt_domain_name[i+3] << 24) + (mqtt_domain_name[i+2] << 16) + (mqtt_domain_name[i+1] << 8) + mqtt_domain_name[i];
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
    memset(nv_db, 0x00, MAX_NVRAM_SIZE * 4);
    for(int i = 0; i < MAX_NVRAM_SIZE; i++) nv_db[i] = *(__IO uint64_t*)(FLASH_USER_START_ADDR + (i * 4));  
    
    mqtt_did_num     = nv_db[NVRAM_SN];
    did_sub_num      = nv_db[NVRAM_SN_SUB];
    longitude        = nv_db[NVRAM_LONGITUDE];
    latitude         = nv_db[NVRAM_LATITUDE];
    report_cycle     = nv_db[NVRAM_REPORT_CYCLE];
    
    if( (longitude == 0) || (latitude == 0) ) gps_status = 0xDEADDEAD;
    
    for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
    {
        int db_index = NVRAM_DOMAIN + (i / 4);
        mqtt_domain_name[i+3] = (char)((nv_db[db_index] & 0xFF000000) >> 24);
        mqtt_domain_name[i+2] = (char)((nv_db[db_index] & 0x00FF0000) >> 16);
        mqtt_domain_name[i+1] = (char)((nv_db[db_index] & 0x0000FF00) >> 8);
        mqtt_domain_name[i] = (char)((nv_db[db_index] & 0x000000FF));
    }
}

void NVRam_Write(void)
{  
    HAL_FLASH_Unlock();
    HAL_Delay(10U);
    for(int i = 0; i < MAX_NVRAM_SIZE; i++)
    {
        __disable_irq();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR + (i * 4), nv_db[i]);
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

int rs_flag = 0;
void do_rs_command(int cnt)
{
    RTC_TimeTypeDef sTime = {0};
    if( (debug_buf[0] == 'S') && (debug_buf[1] == 'N') )
    {
        sscanf(&debug_buf[2],"%d %d", &did_sub_num, &mqtt_did_num);
        if( mqtt_did_num > 999999999 ) mqtt_did_num = 1;
        if( did_sub_num > 999 ) did_sub_num = 0;
        UART_Printf(">>>SN write to NV_DB(%03d-%09d)<<<\n", did_sub_num, mqtt_did_num);
        nv_db[NVRAM_SN]     = mqtt_did_num;
        nv_db[NVRAM_SN_SUB] = did_sub_num;
        rs_flag = 1;
    }
    else if( (debug_buf[0] == 'R') && (debug_buf[1] == 'C') )
    {
        sscanf(&debug_buf[2],"%d", &report_cycle);
        if( report_cycle > MAX_REPORT_TIME ) report_cycle = MAX_REPORT_TIME;
        UART_Printf(">>>Report Cycle write to NV_DB(%d sec)<<<\n", report_cycle);
        nv_db[NVRAM_REPORT_CYCLE] = report_cycle;
        rs_flag = 1;
    }
    else if( (debug_buf[0] == 'G') && (debug_buf[1] == 'A') )
    {
        sscanf(&debug_buf[2],"%d", &latitude);
        nv_db[NVRAM_LATITUDE] = latitude;
        UART_Printf(">>>Latitude write to NV_DB(%d sec)<<<\n", latitude);
        rs_flag = 1;      
    }
    else if( (debug_buf[0] == 'G') && (debug_buf[1] == 'O') )
    {
        sscanf(&debug_buf[2],"%d", &longitude);
        nv_db[NVRAM_LONGITUDE] = longitude;
        UART_Printf(">>>Longitude write to NV_DB(%d sec)<<<\n", longitude);
        rs_flag = 1;
    }
    else if( (debug_buf[0] == 'T') && (debug_buf[1] == 'I') )
    {
        sscanf(&debug_buf[2],"%d:%d:%d", &sTime.Hours, &sTime.Minutes, &sTime.Seconds);
        HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
        UART_Printf("Set Time %02d:%02d:%02d\n",sTime.Hours,sTime.Minutes,sTime.Seconds);
        rs_flag = 0;
    }
    else if( (debug_buf[0] == 'D') && (debug_buf[1] == 'I') )
    {
        UART_Printf("Serial Number : %03d-%09d\n", did_sub_num, mqtt_did_num);
        UART_Printf("Latitude      : %d.%05d\n",(latitude/100000),(latitude%100000));
        UART_Printf("Longitude     : %d.%05d\n",(longitude/100000),(longitude%100000));
        UART_Printf("Report Cycle  : %02d:%02d:%02d\n",report_cycle/3600, (report_cycle%3600)/60, report_cycle%60);
        UART_Printf("Domain Name   : %s\n",mqtt_domain_name);
        rs_flag = 0;
    }
    else if( (debug_buf[0] == 'N') && (debug_buf[1] == 'D') )
    {
        int index = 2;
        while(1){
        if( debug_buf[index] == 0x20 )index++;
        else break;
        }
        sscanf(&debug_buf[index],"%[^$]", mqtt_domain_name);

        index = 0;
        while(1){
        if( mqtt_domain_name[index] == 0x20 ){
            mqtt_domain_name[index] = 0x00;
            break;
        }

        if( mqtt_domain_name[index] == 0x00 ){
            break;
        }
        index++;
        }
        for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
        {
        int db_index = NVRAM_DOMAIN + (i / 4);
        nv_db[db_index] = (mqtt_domain_name[i+3] << 24) + (mqtt_domain_name[i+2] << 16) + (mqtt_domain_name[i+1] << 8) + mqtt_domain_name[i];
        }
        UART_Printf(">>>Domain name write to NV_DB(%s)<<<\n", mqtt_domain_name);
        nv_db[NVRAM_REPORT_CYCLE] = report_cycle;
        rs_flag = 1;
    }
    
    if(rs_flag)
    {
        EraseNV();
        HAL_Delay(100);
        NV_Write();
        HAL_Delay(500);
        NVIC_SystemReset();
    }
}
