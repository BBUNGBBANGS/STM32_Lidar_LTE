#include "os.h"
#include "buzzer.h"
#include "debug.h"
#include "device.h"
#include "dht11.h"
#include "lidar.h"
#include "lte.h"
#include "nvram.h"
#include "rtc.h"

Os_Var_t Os_Var;

static void Os_Time_Handler(void);
static void Os_Handler_10ms(void);
static uint32_t Os_Get_Tick_100us(void);
static void Os_Calc_Period(uint8_t idx);
static void Os_Calc_Runtime_Start(uint8_t idx);
static void Os_Calc_Runtime_Stop(uint8_t idx);
static void Os_10ms_Task(void);
static void Os_100ms_Task(void);
static void Os_Background_Task(void);

void Os_Init_Task(void)
{
	//RTC_Init();
    Device_Init();
    DHT11_Init();
    Debug_Init();
    Lidar_Init();
    LTE_Init();
    Buzzer_Init();
	NVRam_Init();
    Device_Reset_Init();
}

static void Os_10ms_Task(void)
{
	Device_Battery_Voltage_Read();
	Device_Detect_SensorSignal();
}

static void Os_100ms_Task(void)
{
	Lidar_Receive_Data();
	Test_Debug();
	LTE_Control();
	RTC_Measure_Time();
	Device_SleepMode_Start();
}

static void Os_Background_Task(void)
{

}

static void Os_Time_Handler(void)
{
	Os_Var.Os_Counter.Os_100us_Counter++;
}

void Os_Handler(void)
{
    if(Os_Var.Os_Task_Flag.Os_100ms_Flag == 1)
	{
		Os_Var.Os_Task_Flag.Os_100ms_Flag = 0;
		Os_Calc_Runtime_Start(OS_TASK_INDEX_100MS);
		(void)Os_100ms_Task();
		Os_Calc_Runtime_Stop(OS_TASK_INDEX_100MS);
		Os_Calc_Period(OS_TASK_INDEX_100MS);
	}
	else
	{
		Os_Calc_Runtime_Start(OS_TASK_INDEX_BG);
		(void)Os_Background_Task();
		Os_Calc_Runtime_Stop(OS_TASK_INDEX_BG);
		Os_Calc_Period(OS_TASK_INDEX_BG);
	}

    return;
}

static void Os_Handler_10ms(void)
{
	Os_Var.Os_Counter.Os_10ms_Counter++;
	Os_Var.Os_Task_Flag.Os_10ms_Flag = 1;
    
	if(Os_Var.Os_Counter.Os_10ms_Counter % 10 == 0)
	{
		Os_Var.Os_Counter.Os_100ms_Counter++;
		Os_Var.Os_Task_Flag.Os_100ms_Flag = 1;
	}

	void(*fp)(void);
	if(Os_Var.Os_Task_Flag.Os_10ms_Flag == 1)
	{
		Os_Var.Os_Task_Flag.Os_10ms_Flag = 0;
		Os_Calc_Runtime_Start(OS_TASK_INDEX_10MS);
		(void)Os_10ms_Task();
		Os_Calc_Runtime_Stop(OS_TASK_INDEX_10MS);
		Os_Calc_Period(OS_TASK_INDEX_10MS);
	}
}

static uint32_t Os_Get_Tick_100us(void)
{
	return Os_Var.Os_Counter.Os_100us_Counter;
}

static void Os_Calc_Period(uint8_t idx)
{
	uint32_t OS_TimeLoc = 0;
	uint32_t OS_TimeOldLoc = 0;
	uint32_t Os_TimeDiffLoc = 0;

	OS_TimeLoc = Os_Get_Tick_100us();

	switch(idx)
	{
		case OS_TASK_INDEX_10MS : 
			OS_TimeOldLoc = Os_Var.Os_Tick_Period.Os_10ms_Tick_Period;
		break;
		case OS_TASK_INDEX_100MS : 
			OS_TimeOldLoc = Os_Var.Os_Tick_Period.Os_100ms_Tick_Period;
		break;
	}

	Os_TimeDiffLoc = OS_TimeLoc - OS_TimeOldLoc;
	OS_TimeOldLoc = OS_TimeLoc;
	
	switch(idx)
	{
		case OS_TASK_INDEX_10MS : 
			Os_Var.Os_Period.Os_10ms_Period = Os_TimeDiffLoc;
			Os_Var.Os_Tick_Period.Os_10ms_Tick_Period = OS_TimeOldLoc;
		break;
		case OS_TASK_INDEX_100MS : 
			Os_Var.Os_Period.Os_100ms_Period = Os_TimeDiffLoc;
			Os_Var.Os_Tick_Period.Os_100ms_Tick_Period = OS_TimeOldLoc;
		break;
	}
	
	return;
}

static void Os_Calc_Runtime_Start(uint8_t idx)
{
	uint32_t OS_TimeLoc = 0;

	OS_TimeLoc = Os_Get_Tick_100us();

	switch(idx)
	{
		case OS_TASK_INDEX_10MS : 
			Os_Var.Os_Tick_Runtime.Os_10ms_Tick_Runtime_Start = OS_TimeLoc;
		break;
		case OS_TASK_INDEX_100MS : 
			Os_Var.Os_Tick_Runtime.Os_100ms_Tick_Runtime_Start = OS_TimeLoc;
		break;
	}

	return;
}

static void Os_Calc_Runtime_Stop(uint8_t idx)
{
	uint32_t OS_TimeLoc = 0;
	uint32_t OS_TimeOldLoc = 0;
	uint32_t Os_TimeDiffLoc = 0;

	OS_TimeLoc = Os_Get_Tick_100us();

	switch(idx)
	{
		case OS_TASK_INDEX_10MS : 
			OS_TimeOldLoc = Os_Var.Os_Tick_Runtime.Os_10ms_Tick_Runtime_Start;
		break;
		case OS_TASK_INDEX_100MS : 
			OS_TimeOldLoc = Os_Var.Os_Tick_Runtime.Os_100ms_Tick_Runtime_Start;
		break;
	}

	Os_TimeDiffLoc = OS_TimeLoc - OS_TimeOldLoc;

	switch(idx)
	{
		case OS_TASK_INDEX_10MS : 
			Os_Var.Os_Runtime.Os_10ms_Runtime = Os_TimeDiffLoc;
			Os_Var.Os_Tick_Runtime.Os_10ms_Tick_Runtime_Stop = OS_TimeLoc;
		break;
		case OS_TASK_INDEX_100MS : 
			Os_Var.Os_Runtime.Os_100ms_Runtime = Os_TimeDiffLoc;
			Os_Var.Os_Tick_Runtime.Os_100ms_Tick_Runtime_Stop = OS_TimeLoc;
		break;
	}

	return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM17)
    {
        Os_Handler_10ms();
    }
    if(htim->Instance == TIM16)
    {
        Os_Time_Handler();
    }
}