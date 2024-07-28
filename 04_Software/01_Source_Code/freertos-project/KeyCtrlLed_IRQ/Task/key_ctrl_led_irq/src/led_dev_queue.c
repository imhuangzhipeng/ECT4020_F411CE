#include "gpio.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "led_dev_queue.h"
#include "key_irq_queue.h"
#include "uart_printf.h"

// #define DEBUG

extern QueueHandle_t g_key_result_queue_handle;

static uint8_t waitflag = 0;

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

void start_recv_timout_timer(void)
{
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start_IT(&htim4);
}

void led_timeout_callback(void)
{
    waitflag = 0;

    HAL_TIM_Base_Stop_IT(&htim4);

    /* 注意:关闭中断之后的代码不会执行 */
}

void led_task(void)
{
    static key_result_t key_result = KEY_NO_PRESS;

    xQueueReceive(g_key_result_queue_handle, &key_result, portMAX_DELAY);

    switch (key_result)
    {
    case KEY_SHORT_PRESS:
        led_toggle();
        break;
    case KEY_LONG_PRESS:
        /* LED闪烁10次，闪烁频率为精确的5hz，100ms翻转一次LED电平，200ms为LED闪烁的一次周期。*/
        for (int i = 0; i < 20; i++)
        {
            while (waitflag)
            {
                /* code */
            }

            start_recv_timout_timer();

            led_toggle();
#ifdef DEBUG
            osPrintf("Callback Toggle function,loop times: %d.\r\n", i);
#endif
            waitflag = 1;
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
