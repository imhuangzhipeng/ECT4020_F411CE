#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "led_dev_queue.h"
#include "key_irq_queue.h"
#include "uart_printf.h"

extern LedInterface led_interface;
extern KeyInterface key_interface;

static TaskHandle_t keyTaskHandle;
static TaskHandle_t ledTaskHandle;

static void ledTask(void *arg)
{
    led_interface.pfinit();
	
    while (1)
    {
        led_interface.pftask();
    }
}

static void keyTask(void *arg)
{
    key_interface.pfinit();
	
    while (1)
    {
        key_interface.pftask();
    }
}

void key_ctrl_led_irq_tasks_init(void)
{
    BaseType_t isTaskCreate = pdFAIL;

    isTaskCreate = xTaskCreate(ledTask,
                               "led_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &ledTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate key_task failed! \r\n");
        return;
    }

    isTaskCreate = xTaskCreate(keyTask,
                               "ked_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &keyTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate key_task failed! \r\n");
        return;
    }
}
