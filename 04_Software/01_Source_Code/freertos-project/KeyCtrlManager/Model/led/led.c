#include "led_device.h"
#include "mcu_gpio_driver.h"
#include "gpio.h"

/* 定义LED对象私有属性 */
typedef struct led_device_data_t
{
    GPIO_TypeDef *GPIO_PORT;
    uint16_t GPIO_PIN;
    pgpiohandle_t pgpio_fun;
} LedDevData;

/* LED1对象的私有属性初始化 */
static LedDevData __g_led1_device_data = {
    .GPIO_PORT = LED_GPIO_Port,
    .GPIO_PIN = LED_Pin,
    .pgpio_fun = NULL,
};

int ledInit(LedDevice *pled_device)
{
    LedDevData *data = (LedDevData *)pled_device->private_data;

    data->pgpio_fun = get_gpio_handle();
    if (!data->pgpio_fun)
    {
        return -1;
    }

    return 0;
}

int ledDeviceControl(LedDevice *pled_device, LED_STATE led_state)
{
    LedDevData *data = pled_device->private_data;

    if (!pled_device)
    {
        return -1;
    }

    switch (led_state)
    {
    case ON:
        data->pgpio_fun->pfgpio_write_pin(data->GPIO_PORT, data->GPIO_PIN, HIGH_LEVEL);
        pled_device->led_state = ON;
        return 1;
    case OFF:
        data->pgpio_fun->pfgpio_write_pin(data->GPIO_PORT, data->GPIO_PIN, LOW_LEVEL);
        pled_device->led_state = OFF;
        return 0;
    case Toggle:
        data->pgpio_fun->pfgpio_toggle_pin(data->GPIO_PORT, data->GPIO_PIN);
        pled_device->led_state = Toggle;
        return 2;
    default:
        return -1;
    }
}

/* 实例化LED1 设备 */
LedDevice g_led1_device = {
    .led_type = LED_BLUE_D2,
    .led_state = OFF,
    .ledInit = ledInit,
    .ledControl = ledDeviceControl,
    .setLedBrightness = NULL,
    .setLedColor = NULL,
    .private_data = &__g_led1_device_data,
};
