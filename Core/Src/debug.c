#include "debug.h"
#include "uart.h"
#include "lte.h"
#include "nvram.h"
#include "device.h"

uint8_t Debug_Rx_buffer[1024];
uint16_t Debug_Rx_Count;
uint8_t debug_dummy;
uint8_t Debug_Write_Request;
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

void Debug_Clear_Buffer(void)
{
    memset(Debug_Rx_buffer,0x00,Debug_Rx_Count);
    Debug_Rx_Count = 0;
}
void Debug_Write_Variable(void)
{
    RTC_TimeTypeDef Time = {0};
    RTC_DateTypeDef Date = {0};
    uint8_t tx_buf[150] = {0,};
    uint32_t LatLoc,LongLoc;

    if((Debug_Rx_Count>3) && (Debug_Rx_buffer[Debug_Rx_Count-4] == '$') && (Debug_Rx_buffer[Debug_Rx_Count-3] == '$') && (Debug_Rx_buffer[Debug_Rx_Count-2] == '$'))
    {
        if((Debug_Rx_buffer[0] == 'S') && (Debug_Rx_buffer[1] == 'N'))
        {
            sscanf(&Debug_Rx_buffer[2],"%d %d", &LTE_SN, &LTE_SN_SUB);
            if(LTE_SN > 999) 
            {
                LTE_SN = 1;
            }
            if(LTE_SN_SUB > 999999999) 
            {
                LTE_SN_SUB = 0;
            }
            sprintf(tx_buf,">>>SN write to NVRam(%03d-%09d)<<<\n", LTE_SN, LTE_SN_SUB);
            Debug_Message_Transmit(tx_buf);
            NVRAM_Block[NVRAM_SN] = LTE_SN;
            NVRAM_Block[NVRAM_SN_SUB] = LTE_SN_SUB;
            Debug_Clear_Buffer();
            Debug_Write_Request = 1;
        }
        else if((Debug_Rx_buffer[0] == 'R') && (Debug_Rx_buffer[1] == 'C'))
        {
            sscanf(&Debug_Rx_buffer[2],"%d", &Device_Report_Cycle);
            if(Device_Report_Cycle > REPORT_TIME_MAX) 
            {
                Device_Report_Cycle = REPORT_TIME_MAX;
            }
            sprintf(tx_buf,">>>Report Cycle write to NVRam(%d sec)<<<\n", Device_Report_Cycle);
            Debug_Message_Transmit(tx_buf);
            NVRAM_Block[NVRAM_REPORT_CYCLE] = Device_Report_Cycle;
            Debug_Clear_Buffer();
            Debug_Write_Request = 1;
        }
        else if((Debug_Rx_buffer[0] == 'G') && (Debug_Rx_buffer[1] == 'A'))
        {
            sscanf(&Debug_Rx_buffer[2],"%d", &LatLoc);
            LTE_GPS_LAT_I = LatLoc / 100000;
            LTE_GPS_LAT_D = LatLoc % 100000;
            NVRAM_Block[NVRAM_LATITUDE_I] = LTE_GPS_LAT_I;
            NVRAM_Block[NVRAM_LATITUDE_D] = LTE_GPS_LAT_D;
            sprintf(tx_buf,">>>Latitude write to NVRam(%d sec)<<<\n", LTE_GPS_LAT_I);
            Debug_Message_Transmit(tx_buf);
            Debug_Clear_Buffer();
            Debug_Write_Request = 1;      
        }
        else if((Debug_Rx_buffer[0] == 'G') && (Debug_Rx_buffer[1] == 'O'))
        {
            sscanf(&Debug_Rx_buffer[2],"%d", &LongLoc);
            LTE_GPS_LONG_I = LongLoc / 100000;
            LTE_GPS_LONG_D = LongLoc % 100000;
            NVRAM_Block[NVRAM_LONGITUDE_I] = LTE_GPS_LONG_I;
            NVRAM_Block[NVRAM_LONGITUDE_D] = LTE_GPS_LONG_D;
            sprintf(tx_buf,">>>Longitude write to NVRam(%d sec)<<<\n", LTE_GPS_LONG_I);
            Debug_Message_Transmit(tx_buf);
            Debug_Clear_Buffer();
            Debug_Write_Request = 1;
        }
        else if((Debug_Rx_buffer[0] == 'T') && (Debug_Rx_buffer[1] == 'I'))
        {
            HAL_RTC_GetTime(&hrtc, &Time, FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &Date, FORMAT_BIN);
            sscanf(&Debug_Rx_buffer[2],"%d:%d:%d", &Time.Hours, &Time.Minutes, &Time.Seconds);
            HAL_RTC_SetTime(&hrtc, &Time, FORMAT_BIN);
            HAL_RTC_SetDate(&hrtc, &Date, FORMAT_BIN);
            sprintf(tx_buf,"Set Time %02d:%02d:%02d\n",Time.Hours,Time.Minutes,Time.Seconds);
            Debug_Message_Transmit(tx_buf);
            Debug_Clear_Buffer();
            Debug_Write_Request = 0;
        }
        else if((Debug_Rx_buffer[0] == 'D') && (Debug_Rx_buffer[1] == 'I'))
        {
            sprintf(tx_buf,"Serial Number : %03d-%09d\nLatitude      : %d.%05d\nLongitude     : %d.%05d\nReport Cycle  : %02d:%02d:%02d\nDomain Name   : %s\n",
                LTE_SN,LTE_SN_SUB,LTE_GPS_LAT_I,LTE_GPS_LAT_D,LTE_GPS_LONG_I,LTE_GPS_LONG_D,Device_Report_Cycle/3600,(Device_Report_Cycle%3600)/60,Device_Report_Cycle%60,NVRAM_Domain);
            Debug_Message_Transmit(tx_buf);
            Debug_Clear_Buffer();
            Debug_Write_Request = 0;
        }
        else if((Debug_Rx_buffer[0] == 'N') && (Debug_Rx_buffer[1] == 'D'))
        {
            int index = 2;
            while(1)
            {
                if(Debug_Rx_buffer[index] == 0x20)
                {
                    index++;
                }
                else
                {
                    break;
                }
            }
            sscanf(&Debug_Rx_buffer[index],"%[^$]", NVRAM_Domain);

            index = 0;
            while(1)
            {
                if(NVRAM_Domain[index] == 0x20)
                {
                    NVRAM_Domain[index] = 0x00;
                    break;
                }

                if(NVRAM_Domain[index] == 0x00)
                {
                    break;
                }
                index++;
            }
            for(int i = 0; i < MQTT_DOMAIN_NAME_SIZE; i += 4)
            {
                int db_index = NVRAM_DOMAIN + (i / 4);
                NVRAM_Block[db_index] = (NVRAM_Domain[i+3] << 24) + (NVRAM_Domain[i+2] << 16) + (NVRAM_Domain[i+1] << 8) + NVRAM_Domain[i];
            }
            sprintf(tx_buf,">>>Domain name write to NVRam(%s)<<<\n", NVRAM_Domain);
            Debug_Message_Transmit(tx_buf);
            Debug_Clear_Buffer();

            Debug_Write_Request = 1;
        }
    }
    
    if(1 == Debug_Write_Request)
    {
        NVRam_Update_Variable();
    }
}