#ifndef _DEBUG_H
#define _DEBUG_H

#include "main.h"

#define DEBUG_MODULE    huart3

extern uint8_t Debug_Rx_buffer[1024];
extern uint16_t Debug_Rx_Count;

extern void Debug_Init(void);
extern void Debug_Message_Transmit(uint8_t * buf);
extern void Test_Debug(void);

#endif