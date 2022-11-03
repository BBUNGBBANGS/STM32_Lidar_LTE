#ifndef _DEBUG_H
#define _DEBUG_H

#include "main.h"

#define DEBUG_MODULE    huart3

extern void Debug_Init(void);
extern void Debug_Message_Transmit(uint8_t * buf);

#endif