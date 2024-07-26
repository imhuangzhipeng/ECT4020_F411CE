#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "key_task_queue.h"
#include "uart_printf.h"

#define DEBUG

extern QueueHandle_t keyCtrlQueueHandle;

TaskHandle_t keyTaskHandle;

static struct Key_Device *__user_key_handle;
static manage_key_event_t __key_event_val;

int8_t readUserKeyPin(void)
{
    GPIO_PinState pinState;

    pinState = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);

    return (int8_t)pinState;
}

void userKeyPressed(void)
{
    __key_event_val = KEY_PRESS_EVENT;
}

void userKeyLongPressed(void)
{
    __key_event_val = KEY_LONG_EVENT;
}

/* 初始化按键设备 */
void keyDeviceCreate(struct Key_Device **key_handle)
{
    *key_handle = getKeyDevice(USER_KEY);

    if (NULL == key_handle)
    {
        osPrintf("getKeyDevice() failed! \r\n");
        return;
    }

    (*key_handle)->keyInit(*key_handle, RESET, readUserKeyPin, HAL_GetTick);

    (*key_handle)->keyBindingEvent(*key_handle, NONE_PRESS, NULL);

    (*key_handle)->keyBindingEvent(*key_handle, PRESS_DOWN, userKeyPressed);

    (*key_handle)->keyBindingEvent(*key_handle, PRESS_UP, NULL);

    (*key_handle)->keyBindingEvent(*key_handle, PRESS_LONG, userKeyLongPressed);
}

/* 扫描按键状态并发送到消息队列 */
void keyTaskFunction(void *args)
{
    BaseType_t isQueueSend = pdFAIL;

    while (1)
    {
        // 阻塞等待锁
        //  while (keyTaskPauseFlag)
        //  {
        //      //获取锁
        //      //等待锁
        //      //释放锁
        // }

        // 扫描按键是否按下, 获取按键事件
        __user_key_handle->keyScan(__user_key_handle);

        isQueueSend = xQueueSend(keyCtrlQueueHandle,
                                 (void *)&__key_event_val,
                                 200);
#ifdef DEBUG
        osPrintf("__key_event_val = %d \r\n", __key_event_val);
#endif

        if (isQueueSend != pdPASS)
        {
            osPrintf("xQueueSend failed! \r\n");
        }
        else
        {
            vTaskDelay(100);
        }
    }
}

/* 按键任务创建 */
void keyTaskInit(void)
{
    BaseType_t isTaskCreate = pdFAIL;

    keyDeviceCreate(&__user_key_handle);

    isTaskCreate = xTaskCreate(keyTaskFunction,
                               "key_task",
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
