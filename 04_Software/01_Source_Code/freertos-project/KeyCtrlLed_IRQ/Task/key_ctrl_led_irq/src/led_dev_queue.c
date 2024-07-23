#include "gpio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "led_dev_queue.h"
#include "key_irq_queue.h"

extern QueueHandle_t g_key_result_queue_handle;

void led_init(void);
void led_task(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

LedInterface led_interface = {
    .pfinit = led_init,
    .pftask = led_task,
    .pfon = led_on,
    .pfoff = led_off,
    .pftoggle = led_toggle,
};

void led_init(void)
{
    led_on();
}

void led_task(void)
{
    key_result_t key_result = KEY_NO_PRESS;

    xQueueReceive(g_key_result_queue_handle, &key_result, portMAX_DELAY);

    switch (key_result)
    {
    case KEY_SHORT_PRESS:
        led_toggle();
        break;
    case KEY_LONG_PRESS:
        for (int i = 0; i < 6; i++)
        {
            led_toggle();
            vTaskDelay(150);
        }
        break;
    default:
        break;
    }
}

void led_on(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void led_off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void led_toggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
