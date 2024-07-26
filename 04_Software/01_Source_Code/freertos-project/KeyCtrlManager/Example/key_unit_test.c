#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "key_device.h"
#include "uart_printf.h"

// 测试流程:
// 1.keyUnitTestTaskInit()创建按键任务keyStatePrint()
// 2.keyStatePrint()调用getKeyDevice()获取按键句柄
// 3.keyStatePrint()调用Key_Init_OK()绑定按键触发事件的回调函数
// 4.keyStatePrint()轮询按键状态
// 测试结果: 根据按键单击/长按状态printf输出到串口uart1

static struct Key_Device *user_key_handle;

static int8_t Key_ReadPin_State(void)
{
    GPIO_PinState pinState;

    pinState = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);

    return (int8_t)pinState;
}

static void Key_PressedDown_Printf(void)
{
    osPrintf("User Key Pressed Down! \r\n");
    return;
}

static void Key_PressedUP_Printf(void)
{
    osPrintf("User Key Pressed UP! \r\n");
    return;
}

static void Key_LongPressed_Printf(void)
{
    osPrintf("User Key Long Pressed! \r\n");
    return;
}

static void Key_Init_OK(void)
{
    user_key_handle = getKeyDevice(USER_KEY);

    if (NULL == user_key_handle)
    {
        osPrintf("getKeyDevice() failed! \r\n");
        return;
    }

    user_key_handle->keyInit(user_key_handle, RESET, Key_ReadPin_State, HAL_GetTick);

    user_key_handle->keyBindingEvent(user_key_handle, NONE_PRESS, NULL);

    user_key_handle->keyBindingEvent(user_key_handle, PRESS_DOWN, Key_PressedDown_Printf);

    user_key_handle->keyBindingEvent(user_key_handle, PRESS_UP, Key_PressedUP_Printf);

    user_key_handle->keyBindingEvent(user_key_handle, PRESS_LONG, Key_LongPressed_Printf);
}

void keyStatePrint(void *arg)
{
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

    Key_Init_OK();
}
