#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "led_task_queue.h"
#include "mcu_gpio_driver.h"
#include "uart_printf.h"

extern QueueHandle_t ledCtrlQueueHandle;

TaskHandle_t ledTaskHandle;

static struct LED_Device *__led_b1_handle;

void ledToggle(void)
{
    __led_b1_handle->ledControl(__led_b1_handle, Toggle);
}

void ledFlash(void)
{
    int i = 0;

    for (i = 0; i < 6; i++)
    {
        ledToggle();
    }
}

/* 初始化LED设备 */
void ledDeviceCreate(struct LED_Device *led_handle)
{
    led_handle = getLedDevice(LED_BLUE_D2);

    if (NULL == led_handle)
    {
        osPrintf("getLedDevice() failed! \r\n");
        return;
    }

    register_gpio_function(NULL,
                           HAL_GPIO_WritePin,
                           HAL_GPIO_ReadPin,
                           HAL_GPIO_TogglePin);

    led_handle->ledInit(led_handle);
}

/* 接收按键管理任务的消息并根据消息队列的指令执行LED控制函数 */
void ledTaskFunction(void *args)
{
    BaseType_t isQueueRecv = pdFAIL;

    manage_led_ctl_cmd_t led_ctl_cmd = LED_UNDO;

    while (1)
    {
        // receive data from queue
        isQueueRecv = xQueueReceive(ledCtrlQueueHandle,
                                    (void *)&led_ctl_cmd,
                                    portMAX_DELAY);
        if (isQueueRecv != pdPASS)
        {
            osPrintf("xQueueReceive failed! \r\n");
            continue;
        }
#ifdef DEBUG
        osPrintf("led_ctl_cmd = %d \r\n", led_ctl_cmd);
#endif

        switch (led_ctl_cmd)
        {
        case LED_TOGGLE:
            ledToggle();
            break;

        case LED_FLASH:
            ledFlash();
            break;

        default:
            break;
        }
    }
}

/* 创建LED任务 */
void ledTaskInit(void)
{
    BaseType_t isTaskCreate = pdFAIL;

    ledDeviceCreate(__led_b1_handle);

    isTaskCreate = xTaskCreate(ledTaskFunction,
                               "led_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &ledTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate led_task failed! \r\n");
        return;
    }
}
