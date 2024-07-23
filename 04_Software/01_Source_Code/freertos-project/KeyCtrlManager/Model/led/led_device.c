#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#include "led_device.h"

extern LedDevice g_led1_device;

static LedDevice *__g_led_device_set[LED_TYPE_MAX] = {
    &g_led1_device,
};

pLedDevice getLedDevice(LED_TYPE led_type)
{
    int i = 0;
    for (i = 0; i < LED_TYPE_MAX; i++)
    {
        if (led_type == __g_led_device_set[i]->led_type)
            return __g_led_device_set[i];
    }

    return NULL;
}
