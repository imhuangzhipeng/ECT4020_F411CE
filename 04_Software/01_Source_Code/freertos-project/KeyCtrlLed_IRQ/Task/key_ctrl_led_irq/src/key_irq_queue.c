#include <stdint.h>
#include "gpio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "key_irq_queue.h"
#include "uart_printf.h"

#define KEY_RESULT_QUEUE_SIZE 10
#define KEY_EVENT_QUEUE_SIZE 10

typedef struct
{
    uint32_t triggle_tick;
    key_state_t key_state;
} key_event_t;

QueueHandle_t g_key_event_queue_handle = NULL;
QueueHandle_t g_key_result_queue_handle = NULL;

void keyInit(void)
{

    g_key_event_queue_handle = xQueueCreate(KEY_EVENT_QUEUE_SIZE, sizeof(key_event_t));
    g_key_result_queue_handle = xQueueCreate(KEY_RESULT_QUEUE_SIZE, sizeof(key_result_t));

    if (!g_key_event_queue_handle || !g_key_result_queue_handle)
    {
        osPrintf("keyInit() failed: xQueueCreate!  \r\n");
        return;
    }
}

void keyTask(void)
{
    BaseType_t isQueueRetFail = pdFAIL;

    key_state_t key_state = KEY_NULL;
    key_result_t key_result = KEY_NO_PRESS;
    key_event_t key_event = {0};

    static uint32_t __press_time = 0;

    // 阻塞等待, 接收上升沿/下降沿中断消息
    isQueueRetFail = xQueueReceive(g_key_event_queue_handle, &key_event, portMAX_DELAY);

    if (isQueueRetFail != pdPASS)
    {
        osPrintf("keyTask() xQueueReceive failed! \r\n");
    }

    key_state = key_event.key_state;

    switch (key_state)
    {
    case KEY_PRESS:
        __press_time = key_event.triggle_tick;
        break;
    case KEY_RELEASE:
        __press_time = key_event.triggle_tick - __press_time;
        if (__press_time < 50)
        {
            // 抖动
        }
        else if (50 < __press_time && __press_time < 1000)
        {
            // 短按
            key_result = KEY_SHORT_PRESS;

            // 发送按键控制LED消息
            isQueueRetFail = xQueueSendToBack(g_key_result_queue_handle, &key_result, 200);
            if (isQueueRetFail != pdPASS)
            {
                osPrintf("keyTask() xQueueSendToBack failed! \r\n");
            }
        }
        else
        {
            // 长按
            key_result = KEY_LONG_PRESS;

            isQueueRetFail = xQueueSendToBack(g_key_result_queue_handle, &key_result, 200);
            if (isQueueRetFail != pdPASS)
            {
                osPrintf("keyTask() xQueueSendToBack failed! \r\n");
            }
        }
        break;
    default:
        break;
    }
}

void sendKeyEvent(key_state_t key_state)
{
    static key_event_t __key_event;

    BaseType_t isQueueSend = pdFAIL;

    // 检查是否有更高优先级的任务被唤醒
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (key_state != __key_event.key_state)
    {
        // 组装事件消息
        __key_event.key_state = key_state;
        __key_event.triggle_tick = HAL_GetTick();

        // EXIT0中断发送上升沿/下降沿中断消息
        isQueueSend = xQueueSendFromISR(g_key_event_queue_handle, &__key_event, &xHigherPriorityTaskWoken);
        if (isQueueSend != pdPASS)
        {
            osPrintf("keyTask() xQueueSendToBack failed! \r\n");
        }

        // 如果有更高优先级的任务被唤醒，进行任务切换
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin))
    {
        sendKeyEvent(KEY_RELEASE);
    }
    else
    {
        sendKeyEvent(KEY_PRESS);
    }
}

KeyInterface key_interface = {
    .pfinit = keyInit,
    .pftask = keyTask,
};
