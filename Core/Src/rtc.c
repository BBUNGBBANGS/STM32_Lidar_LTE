#include "rtc.h"
#include "device.h"

uint32_t time_counters;
uint16_t RTC_hours,RTC_minutes,RTC_seconds;

void RTC_Init(void)
{

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, FORMAT_BIN);
    
    
    //Debug_Message_Transmit("Set RTC Alarm\n");
}

void RTC_Measure_Time(void)
{
    uint8_t tx_buf[20] = {0,};
    RTC_TimeTypeDef Time;
    RTC_DateTypeDef Date;
    HAL_RTC_GetTime(&hrtc, &Time, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &Date, FORMAT_BIN);
    RTC_hours = Time.Hours;
    RTC_minutes = Time.Minutes;
    RTC_seconds = Time.Seconds;
    sprintf(tx_buf,"RTC Time : %d:%d:%d\n",RTC_hours,RTC_minutes,RTC_seconds);
    Debug_Message_Transmit(tx_buf);
}
    
void RTC_AlarmConfig(void)
{
    RTC_AlarmTypeDef salarmstructure = {0};
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    uint8_t tx_buf[50] = {0,};
    
    HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, FORMAT_BIN);
    sprintf(tx_buf,"Set Alarm %02d:%02d:%02d\n",sTime.Hours,sTime.Minutes,sTime.Seconds);
    Debug_Message_Transmit(tx_buf);
    /*##-3- Configure the RTC Alarm peripheral #################################*/
    /* RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
    salarmstructure.AlarmTime.Hours   = sTime.Hours + (Device_Report_Cycle / 3600); // Sleep Time 설정
    salarmstructure.AlarmTime.Minutes = sTime.Minutes + ((Device_Report_Cycle / 60) % 60);   
    salarmstructure.AlarmTime.Seconds = sTime.Seconds + (Device_Report_Cycle % 60);
    salarmstructure.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    salarmstructure.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    salarmstructure.AlarmMask = RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES|RTC_ALARMMASK_SECONDS;
    salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    salarmstructure.AlarmDateWeekDay = 1;
    salarmstructure.Alarm = RTC_ALARM_A;

    if( salarmstructure.AlarmTime.Seconds >= 60 )
    {
        salarmstructure.AlarmTime.Seconds -= 60;
        salarmstructure.AlarmTime.Minutes++;
    }
    
    if( salarmstructure.AlarmTime.Minutes >= 60 )
    {
        salarmstructure.AlarmTime.Minutes -= 60;
        salarmstructure.AlarmTime.Hours++;
    }
    
    if( salarmstructure.AlarmTime.Hours >= 24 )
    {
        salarmstructure.AlarmTime.Hours -= 24;
    }
    
    sprintf(tx_buf,"Will Wake-up %02d:%02d:%02d\n",salarmstructure.AlarmTime.Hours,salarmstructure.AlarmTime.Minutes,salarmstructure.AlarmTime.Seconds);
    Debug_Message_Transmit(tx_buf);

    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, Device_Report_Cycle, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
    {
        Error_Handler();
    }
}
