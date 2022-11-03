#ifndef _UART_H
#define _UART_H

#include "main.h"

#define UART_RX_BUFFER_SIZE   1024

typedef struct 
{
  volatile uint32_t input_p;
  volatile uint32_t output_p;
  uint8_t buffer[UART_RX_BUFFER_SIZE];
  uint8_t rxd;
} Uart_Rx_t;

#endif