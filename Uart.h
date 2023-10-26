/*
 * Uart.h
 *
 *  Created on: Sep 24, 2023
 *      Author: Mahmoud Badr
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

typedef enum
{
    UART_0=0,
    UART_1,
    UART_2,
    UART_3,
    UART_4,
    UART_5,
    UART_6,
    UART_7
}UART_t;

void UART_Init(UART_t Copy_UartNum);

void UART_SendBytes(UART_t Copy_UartNum,uint8_t* Copy_Data,uint8_t Copy_DataLength);

void UART_ReceiveBytes(UART_t Copy_UartNum,uint8_t* Copy_Data,uint8_t Copy_DataLength);

#endif /* UART_H_ */
