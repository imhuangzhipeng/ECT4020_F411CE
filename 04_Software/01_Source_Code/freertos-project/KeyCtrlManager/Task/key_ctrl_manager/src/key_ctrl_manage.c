#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "uart_printf.h"
#include "key_ctrl_manage.h"
#include "key_task_queue.h"
#include "led_task_queue.h"

// #define DEBUG

#define QUEUE_LENGTH 10

extern TaskHandle_t keyTaskHandle;
extern TaskHandle_t ledTaskHandle;

extern void keyTaskInit(void);
extern void ledTaskInit(void);

TaskHandle_t keyCtrlTaskHandle;
QueueHandle_t keyCtrlQueueHandle;
QueueHandle_t ledCtrlQueueHandle;

/* 按键事件处理 */
void keyCtrlOperate(manage_key_event_t recv_key_event)
{
    BaseType_t isQueueSend = pdFAIL;

    manage_led_ctl_cmd_t led_ctrl_cmd = LED_UNDO;

    switch (recv_key_event)
    {
    case NONE_EVENT:
    {
        led_ctrl_cmd = LED_UNDO;
    }
    break;
    case KEY_PRESS_EVENT:
    {
        led_ctrl_cmd = LED_TOGGLE;

        isQueueSend = xQueueSend(ledCtrlQueueHandle,
                                 (void *)&led_ctrl_cmd,
                                 200);

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
#ifdef DEBUG
        osPrintf("Send led ctrl cmd: %d.\r\n", led_ctrl_cmd);
#endif
    }
    break;
    case KEY_LONG_EVENT:
    {
        led_ctrl_cmd = LED_FLASH;

        isQueueSend = xQueueSend(ledCtrlQueueHandle,
                                 (void *)&led_ctrl_cmd,
                                 200);

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
#ifdef DEBUG
        osPrintf("Send led ctrl cmd:%d.\r\n", led_ctrl_cmd);
#endif
    }
    break;
    default:
        break;
    }
}

/* 按键事件管理任务 */
void keyCtrlManagerTask(void *args)
{
    BaseType_t isQueueRecv = pdFAIL;

    manage_key_event_t recv_key_event = NONE_EVENT;

    while (1)
    {
        // receive data from queue
        isQueueRecv = xQueueReceive(keyCtrlQueueHandle,
                                    (void *)&recv_key_event,
                                    portMAX_DELAY);
        if (isQueueRecv != pdPASS)
        {
            osPrintf("xQueueReceive failed! \r\n");
            continue;
        }
#ifdef DEBUG
        osPrintf("Recv key event: %d.\r\n", recv_key_event);
#endif

        keyCtrlOperate(recv_key_event);
    }
}

/* 按键任务管理器初始化 */
void keyCtrlManagerInit(void)
{
    BaseType_t isTaskCreate = pdFAIL;

    /* 创建按键消息队列 */
    keyCtrlQueueHandle = xQueueCreate(QUEUE_LENGTH, 1);

    if (!keyCtrlQueueHandle)
    {
        osPrintf("xQueueCreate:keyCtrlQueueHandle failed! \r\n");
        return;
    }

    /* 创建LED消息队列*/
    ledCtrlQueueHandle = xQueueCreate(QUEUE_LENGTH, 1);

    if (!ledCtrlQueueHandle)
    {
        osPrintf("xQueueCreate:ledCtrlQueueHandle failed! \r\n");
        return;
    }

    /* 创建按键控制管理任务 */
    isTaskCreate = xTaskCreate(keyCtrlManagerTask,
                               "key_ctrl_manager_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &keyCtrlTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate key_ctrl_manager_task failed! \r\n");
        return;
    }

    /* 创建按键任务 */
    keyTaskInit();

    /* 创建LED任务 */
    ledTaskInit();

    return;
}

/* 释放任务资源 */
void keyCtrManagerDelete(void)
{
    vTaskDelete(keyTaskHandle);
    vTaskDelete(ledTaskHandle);

    vQueueDelete(keyCtrlQueueHandle);
    vQueueDelete(ledCtrlQueueHandle);

    vTaskDelete(NULL);

    return;
}
