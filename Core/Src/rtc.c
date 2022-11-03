#include "rtc.h"

uint32_t time_counters;



uint16_t RTC_hours,RTC_minutes,RTC_seconds;

void RTC_Init(void)
{
    //sTime.Hours = 3;
    //sTime.Minutes = 28;
    //sTime.Seconds = 50;

    /* Alarm every minute @ XX:XX:20 */
    //sAlarm.AlarmTime.Seconds = 0x20;
    //sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS |RTC_ALARMMASK_MINUTES;
    //sAlarm.Alarm = RTC_ALARM_A;


    //HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
    //HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
}

void RTC_Measure_Time(void)
{
    uint32_t timeLoc;
    
    RTC_TimeTypeDef Time;
    RTC_DateTypeDef Date;
    HAL_RTC_GetTime(&hrtc, &Time, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &Date, FORMAT_BIN);
    RTC_hours = Time.Hours;
    RTC_minutes = Time.Minutes;
    RTC_seconds = Time.Seconds;

    #if 0
    g_diff_time = local_time_counters - time_counters;
    if( g_diff_time < 0 ) g_diff_time += ( 24 * 3600 );
    if( g_diff_time > 120 ) 
    {
         NVIC_SystemReset();
    }
    #endif
}
    

void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
	//HAL_RTCEx_GetTimeStamp(hrtc, &stampTime, &stampDate, RTC_FORMAT_BCD);
}

