#include "buzzer.h"

static void Buzzer_Act(int time);

void Buzzer_Init(void)
{
    // Buzzer PWM Start
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,0);
    
	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
    {
		Error_Handler();
	}

	// Start Buzzer
    Buzzer_Act(100);
}

static void Buzzer_Act(int time)
{
	__HAL_TIM_SET_AUTORELOAD(&htim3, 370);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,50);
	HAL_Delay(time);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,0);
}
