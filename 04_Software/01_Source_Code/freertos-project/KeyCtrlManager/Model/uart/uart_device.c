#include <stdio.h>
#include <string.h>
#include "uart_device.h"

extern struct UART_Device g_uart1_dev;
extern struct UART_Device g_uart2_dev;
extern struct UART_Device g_uart4_dev;

static struct UART_Device *__uart_devices[] = {&g_uart1_dev, &g_uart2_dev, &g_uart4_dev};

struct UART_Device *getUARTDevice(char *name)
{
    int i = 0;
    for (i = 0; i < sizeof(__uart_devices) / sizeof(__uart_devices[0]); i++)
    {
        if (!strcmp(name, __uart_devices[i]->name))
            return __uart_devices[i];
    }

    return NULL;
}