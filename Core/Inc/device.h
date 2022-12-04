#ifndef _DEVICE_H
#define _DEVICE_H

#include "main.h"

typedef enum 
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    RESET_CAUSE_LS_OSC_READY,
} Reset_Cause_t;

#define DEVICE_BATTERY_MODULE  hadc2

#define DEVICE_SENSOR_OFF   0
#define DEVICE_SENSOR_ON    1
#define DEVICE_SENSOR_DETECTED      1
#define DEVICE_SENSOR_ON_DEBOUNCE_TIME      20 //200[ms]

extern uint16_t Device_Batt_Voltage;
extern uint8_t Device_Sensor_Signal;

extern void Device_Init(void);
extern void Device_Reset_Init(void);
extern void Device_Battery_Voltage_Read(void);
extern void Device_Detect_SensorSignal(void);

#endif