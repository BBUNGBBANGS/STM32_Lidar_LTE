#ifndef _RTC_H
#define _RTC_H

#include "main.h"

extern uint16_t RTC_hours,RTC_minutes,RTC_seconds;

extern void RTC_Init(void);
extern void RTC_Measure_Time(void);
extern void RTC_AlarmConfig(void);
#endif