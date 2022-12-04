#ifndef _LTE_H
#define _LTE_H

#include "main.h"

#define LTE_RX_BUFFER_SIZE          (1024)

#define LTE_UART_STATUS_TRANSMIT    (0)
#define LTE_UART_STATUS_RECEIVE     (1)

#define LTE_DELAY_ING               (0)
#define LTE_DELAY_FINISH            (1)

#define LTE_STEP_ATE0               (0)
#define LTE_STEP_CMEE               (1)
#define LTE_STEP_CFUN_DISABLE       (2)
#define LTE_STEP_APN                (3)
#define LTE_STEP_CFUN_ENABLE        (4)
#define LTE_STEP_CESQ               (5)
#define LTE_STEP_CEREG              (6)
#define LTE_STEP_UPSD_MAP           (7)
#define LTE_STEP_UPSD_IPV4          (8)
#define LTE_STEP_UPSDA              (9)
#define LTE_STEP_UPSND              (10)
#define LTE_STEP_MQTT_IP_SET        (11)
#define LTE_STEP_MQTT_DURATION      (12)
#define LTE_STEP_MQTT_SNNV          (13)
#define LTE_STEP_MQTT_CONNECT       (14)
#define LTE_STEP_MQTT_REGISTER      (15)
#define LTE_STEP_MQTT_SUBSCRIBE     (16)
#define LTE_STEP_MQTT_PUBLISH       (17)

#define LTE_GPS_STEP_ULOCCELL       (0)
#define LTE_GPS_STEP_ULOC           (1)
#define LTE_GPS_STEP_UPDATEDATA     (2)
#define LTE_GPS_STEP_FINISH         (3)

typedef enum
{
    FAIL = 0,
    OK = 1
}LTE_Status_t;


extern uint8_t LTE_Rx_Buffer[LTE_RX_BUFFER_SIZE];
extern uint16_t LTE_Rx_Counter;

extern void LTE_Init(void);
extern void LTE_Control(void);

#endif