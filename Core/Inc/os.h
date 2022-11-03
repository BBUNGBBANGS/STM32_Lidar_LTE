#ifndef _OS_H
#define _OS_H

#include "main.h"

typedef struct
{
	uint32_t Os_100us_Counter;
	uint32_t Os_10ms_Counter;
	uint32_t Os_100ms_Counter;
}Os_Counter_t;

typedef struct
{
	uint32_t Os_10ms_Tick_Runtime_Start;
	uint32_t Os_10ms_Tick_Runtime_Stop;
	uint32_t Os_100ms_Tick_Runtime_Start;
	uint32_t Os_100ms_Tick_Runtime_Stop;
}Os_Tick_Runtime_t;

typedef struct
{
	uint32_t Os_10ms_Tick_Period;
	uint32_t Os_100ms_Tick_Period;
}Os_Tick_Period_t;

typedef struct
{
	uint32_t Os_10ms_Runtime;
	uint32_t Os_100ms_Runtime;
}Os_Runtime_t;

typedef struct
{
	uint32_t Os_10ms_Period;
	uint32_t Os_100ms_Period;
}Os_Period_t;

typedef struct 
{
	uint32_t OS_CPU_Load;
	uint32_t OS_CPU_Load_Max;
}OS_CPU_Load_t;

typedef struct 
{
	uint8_t Os_10ms_Flag;
	uint8_t Os_100ms_Flag;
}Os_Task_Flag_t;

typedef struct
{
	uint32_t *Os_10ms_Address;
	uint32_t *Os_100ms_Address;
}Os_Task_Add_t;

typedef struct 
{
	Os_Counter_t Os_Counter;
	Os_Tick_Runtime_t Os_Tick_Runtime;
	Os_Tick_Period_t Os_Tick_Period;
	Os_Runtime_t Os_Runtime;
	Os_Period_t Os_Period;
	OS_CPU_Load_t OS_CPU_Load;
	Os_Task_Flag_t Os_Task_Flag;
	Os_Task_Add_t Os_Task_Add;
}Os_Var_t;

#define OS_TASK_INDEX_10MS			(2u)
#define OS_TASK_INDEX_100MS			(3u)
#define OS_TASK_INDEX_BG			(4u)

extern void Os_Init_Task(void);
extern void Os_Handler(void);

#endif