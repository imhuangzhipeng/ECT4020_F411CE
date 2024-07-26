#include "led_device.h"
#include "mcu_gpio_driver.h"
#include "gpio.h"

/* 定义LED对象私有属性 */
struct led_private_data_t
{
    GPIO_TypeDef *GPIO_PORT;
    uint16_t GPIO_PIN;
    pgpio_interface_t gpio;
};

/* LED1对象的私有属性初始化 */
static struct led_private_data_t __led1_device_data = {
    .GPIO_PORT = LED_GPIO_Port,
    .GPIO_PIN = LED_Pin,
    .gpio = NULL,
};

int ledInit(struct LED_Device *pDev)
{
    struct led_private_data_t *data = (struct led_private_data_t *)pDev->priv_data;

    data->gpio = get_gpio_handle();
    if (!data->gpio)
    {
        return -1;
    }

    return 0;
}

int ledControl(struct LED_Device *pDev, led_state_t state)
{
    struct led_private_data_t *data = pDev->priv_data;

    if (!pDev)
    {
        return -1;
    }

    switch (state)
    {
    case ON:
        data->gpio->pfgpio_write_pin(data->GPIO_PORT, data->GPIO_PIN, HIGH_LEVEL);
        pDev->state = ON;
        return 1;
    case OFF:
        data->gpio->pfgpio_write_pin(data->GPIO_PORT, data->GPIO_PIN, LOW_LEVEL);
        pDev->state = OFF;
        return 0;
    case Toggle:
        data->gpio->pfgpio_toggle_pin(data->GPIO_PORT, data->GPIO_PIN);
        pDev->state = Toggle;
        return 2;
    default:
        return -1;
    }
}

/* 实例化LED1 设备 */
struct LED_Device g_led1_device = {
    .type = LED_BLUE_D2,
    .state = OFF,
    .ledInit = ledInit,
    .ledControl = ledControl,
    .setLedBrightness = NULL,
    .setLedColor = NULL,
    .priv_data = &__led1_device_data,
};
