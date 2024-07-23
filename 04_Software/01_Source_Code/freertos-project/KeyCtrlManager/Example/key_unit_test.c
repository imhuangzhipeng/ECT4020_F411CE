#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "key_device.h"
#include "uart_printf.h"

/**
 * 单元测试函数名命名规范:
 * 功能_条件_期望
 * 
 * 例如:
 * Process_OrderUnshipped_SetShippment()
 * 
 */

static int8_t Key_ReadPin_State(void)
{
    GPIO_PinState pinState;

    pinState = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);

    return (int8_t)pinState;
}

static void Key_Pressed_Printf(void)
{
    osPrintf("User Key Pressed! \r\n");
    return;
}

static void Key_LongPressed_Printf(void)
{
    osPrintf("User Key Long Pressed! \r\n");
    return;
}

static void Key_Init_OK(KeyDevice *key_handle)
{

    key_handle->keyInit(key_handle,
                        RESET,
                        Key_ReadPin_State,
                        HAL_GetTick);

    key_handle->keyBindingEvent(key_handle,
                                NONE_PRESS,
                                NULL);

    key_handle->keyBindingEvent(key_handle,
                                PRESS_DOWN,
                                Key_Pressed_Printf);

    key_handle->keyBindingEvent(key_handle,
                                PRESS_UP,
                                NULL);

    key_handle->keyBindingEvent(key_handle,
                                PRESS_LONG,
                                Key_LongPressed_Printf);
}

void keyStatePrint(void *arg)
{
    KeyDevice *user_key_handle;

    user_key_handle = getKeyDevice(USER_KEY);

    if (NULL == user_key_handle)
    {
        osPrintf("getKeyDevice() failed! \r\n");
        return;
    }

    Key_Init_OK(user_key_handle);

    while (1)
    {
        user_key_handle->keyScan(user_key_handle);
    }
}

void keyUnitTestTaskInit(void)
{
    TaskHandle_t keyTaskHandle;

    BaseType_t isTaskCreate = pdFAIL;

    isTaskCreate = xTaskCreate(keyStatePrint,
                               "unit_test_key_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &keyTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate unit_test_key_task failed! \r\n");
        return;
    }
}
