#include "mcu_gpio_driver.h"
#include "gpio.h"

static gpio_interface_t __g_gpio_handle = {
    .pfgpio_init = NULL,
    .pfgpio_read_pin = NULL,
    .pfgpio_write_pin = NULL,
    .pfgpio_toggle_pin = NULL,
};

// 定义一个函数，用于注册gpio功能
int8_t register_gpio_function(void (*pfgpio_init)(void),
                              void (*pfgpio_write_pin)(void *gpio_port, uint16_t gpio_pin, gpio_pinstate pinstate),
                              int8_t (*pfgpio_read_pin)(void *gpio_port, uint16_t gpio_pin, gpio_pinstate pinstate),
                              void (*pfgpio_toggle_pin)(void *gpio_port, uint16_t gpio_pin))
{
    // 将参数赋值给__g_gpio_handle结构体
    __g_gpio_handle.pfgpio_init = pfgpio_init;
    __g_gpio_handle.pfgpio_write_pin = pfgpio_write_pin;
    __g_gpio_handle.pfgpio_read_pin = pfgpio_read_pin;
    __g_gpio_handle.pfgpio_toggle_pin = pfgpio_toggle_pin;

    // 如果pfgpio_write_pin、pfgpio_read_pin、pfgpio_toggle_pin都不为空
    if ((__g_gpio_handle.pfgpio_write_pin != NULL) &&
        (__g_gpio_handle.pfgpio_read_pin != NULL) &&
        (__g_gpio_handle.pfgpio_toggle_pin != NULL))
    {
        // 返回0
        return 0;
    }
    // 否则返回-1
    return -1;
}

pgpio_interface_t get_gpio_handle(void)
{
    return &__g_gpio_handle;
}
