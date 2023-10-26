#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "Uart.h"
#include "Bootloader/Bootloader.h"
int main(void)
{
    UART_Init(UART_0);
    while(1)
    {
        BL_FetchHostCommand();
    }
}
