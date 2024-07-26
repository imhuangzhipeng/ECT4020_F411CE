#include "led_device.h"
#include <stddef.h>

extern struct LED_Device g_led1_device;

static struct LED_Device *__led_device_set[LED_TYPE_NUM] = {
    &g_led1_device,
};

struct LED_Device *getLedDevice(led_type_t type)
{
    int i = 0;
    for (i = 0; i < LED_TYPE_NUM; i++)
    {
        if (type == __led_device_set[i]->type)
            return __led_device_set[i];
    }

    return NULL;
}
