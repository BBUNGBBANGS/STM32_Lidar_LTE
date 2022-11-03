#ifndef _DHT11_H
#define _DHT11_H

#include "main.h"
 
#define DHT_TIMEOUT 				10000
#define DHT_TIMEOUT2 				90000
#define DHT_POLLING_CONTROL			1 
#define DHT_POLLING_INTERVAL_DHT11	2000 
#define DHT_POLLING_INTERVAL_DHT22	1000 
 
typedef struct 
{
	float Humidity;
	float Temperature;
} DHT_Data_t;

typedef enum
{
    OUTPUT_MODE = 0,
    INPUT_MODE
} DHT_Mode_t;
 
typedef enum 
{
	DHT11 = 0,
	DHT22
} DHT_type_t;
 
extern void DHT11_Init(void);
extern DHT_Data_t DHT_getData(DHT_type_t type) ;  

#endif