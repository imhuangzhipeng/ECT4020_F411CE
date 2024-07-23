#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "uart_printf.h"
#include "key_ctrl_manage.h"
#include "key_task_queue.h"
#include "led_task_queue.h"

#define QUEUE_LENGTH 10

extern TaskHandle_t keyTaskHandle;
extern TaskHandle_t ledTaskHandle;

extern void keyTaskInit(void);
extern void ledTaskInit(void);

TaskHandle_t keyCtrlTaskHandle;
QueueHandle_t keyCtrlQueueHandle;
QueueHandle_t ledCtrlQueueHandle;

void keyCtrlOperate(MANAGE_KEY_EVENT recvFromKeyCtrlQueueVal)
{
    BaseType_t isQueueSend = pdFAIL;

    MANAGE_LED_CTRL ledCtrlCmd = LED_UNDO;

    switch (recvFromKeyCtrlQueueVal)
    {
    case NONE_EVENT:
    {
        ledCtrlCmd = LED_UNDO;

        isQueueSend = xQueueSend(ledCtrlQueueHandle,
                                 (void *)&ledCtrlCmd,
                                 200);

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
    }
    break;
    case KEY_PRESS_EVENT:
    {
        ledCtrlCmd = LED_UNDO;

        isQueueSend = xQueueSend(ledCtrlQueueHandle,
                                 (void *)&ledCtrlCmd,
                                 200);

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
    }
    break;
    case KEY_LONG_EVENT:
    {
        ledCtrlCmd = LED_UNDO;

        isQueueSend = xQueueSend(ledCtrlQueueHandle,
                                 (void *)&ledCtrlCmd,
                                 200);

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
    }
    break;
    default:
        break;
    }
}

void keyCtrlManager(void *args)
{
    BaseType_t isQueueRecv = pdFAIL;

    MANAGE_KEY_EVENT recvFromKeyCtrlQueueVal = NONE_EVENT;

    while (1)
    {
        // receive data from queue
        isQueueRecv = xQueueReceive(keyCtrlQueueHandle,
                                    (void *)&recvFromKeyCtrlQueueVal,
                                    portMAX_DELAY);
        if (isQueueRecv != pdPASS)
        {
            osPrintf("xQueueReceive failed! \r\n");
            continue;
        }
#ifdef DEBUG
        osPrintf("recvFromKeyCtrlQueueVal = %d \r\n", recvFromKeyCtrlQueueVal);
#endif

        keyCtrlOperate(recvFromKeyCtrlQueueVal);
    }
}

void keyCtrlManagerInit(void)
{
    BaseType_t isTaskCreate = pdFAIL;

    keyCtrlQueueHandle = xQueueCreate(QUEUE_LENGTH, 1);

    if (!keyCtrlQueueHandle)
    {
        osPrintf("xQueueCreate:keyCtrlQueueHandle failed! \r\n");
        return;
    }

    ledCtrlQueueHandle = xQueueCreate(QUEUE_LENGTH, 1);

    if (!ledCtrlQueueHandle)
    {
        osPrintf("xQueueCreate:ledCtrlQueueHandle failed! \r\n");
        return;
    }

    isTaskCreate = xTaskCreate(keyCtrlManager,
                               "key_ctrl_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &keyCtrlTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate key_ctrl_task failed! \r\n");
        return;
    }

    keyTaskInit();
    ledTaskInit();

    return;
}

void keyCtrManagerDelete(void)
{
    vTaskDelete(keyTaskHandle);
    vTaskDelete(ledTaskHandle);

    vQueueDelete(keyCtrlQueueHandle);
    vQueueDelete(ledCtrlQueueHandle);

    vTaskDelete(NULL);

    return;
}
