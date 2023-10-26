/*
 * Uart.h
 *
 *  Created on: Sep 24, 2023
 *      Author: Mahmoud Badr
 */
#include "Uart.h"

void UART_Init(UART_t Copy_UartNum)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    UARTDisable(UART0_BASE);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,UART_CONFIG_PAR_NONE | UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE);
    UARTEnable(UART0_BASE);
}

void UART_SendBytes(UART_t Copy_UartNum,uint8_t* Copy_Data,uint8_t Copy_DataLength)
{
    uint8_t Local_Counter=0;
    for( ;Local_Counter<Copy_DataLength;Local_Counter++)
    {
        UARTCharPut((UART0_BASE+Copy_UartNum), Copy_Data[Local_Counter]);
    }
}

void UART_ReceiveBytes(UART_t Copy_UartNum,uint8_t* Copy_Data,uint8_t Copy_DataLength)
{
    uint8_t Local_Counter=0;
    for( ;Local_Counter<Copy_DataLength;Local_Counter++)
    {
        Copy_Data[Local_Counter]=UARTCharGet((UART0_BASE+Copy_UartNum));
    }
}

