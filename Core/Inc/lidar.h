#ifndef _LIDAR_H
#define _LIDAR_H

#include "main.h"

#define LIDAR_MODULE    huart2

#define LIDAR_CHECKSUM_FAIL 0
#define LIDAR_CHECKSUM_OK   1

extern uint16_t Lidar_Distance;
extern uint16_t Lidar_Strength;
extern uint16_t Lidar_Temperature;

extern void Lidar_Init(void);
extern void Lidar_Receive_Data(void);

#endif