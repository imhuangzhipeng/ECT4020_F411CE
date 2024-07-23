#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "led_device.h"
#include "uart_printf.h"
#include "mcu_gpio_driver.h"

void Led_Toggle_Flash(void *arg)
{
    LedDevice *led_handle;

    led_handle = getLedDevice(LED_BLUE_D2);

    if (NULL == led_handle)
    {
        osPrintf("getLedDevice() failed! \r\n");
        return;
    }

    register_gpio_funtion(NULL,
						  HAL_GPIO_WritePin,
						  HAL_GPIO_ReadPin,
						  HAL_GPIO_TogglePin);

    led_handle->ledInit(led_handle);

    while (1)
    {
        led_handle->ledControl(led_handle, Toggle);
        osPrintf("LED_BLUE_D2 Toggle! \r\n");
        vTaskDelay(1000);
    }
}

void ledUnitTestTaskInit(void)
{
    TaskHandle_t ledTaskHandle;

    BaseType_t isTaskCreate = pdFAIL;

    isTaskCreate = xTaskCreate(Led_Toggle_Flash,
                               "unit_test_led_task",
                               128,
                               NULL,
                               osPriorityNormal,
                               &ledTaskHandle);

    if (isTaskCreate != pdPASS)
    {
        osPrintf("xTaskCreate unit_test_led_task failed! \r\n");
        return;
    }
}
