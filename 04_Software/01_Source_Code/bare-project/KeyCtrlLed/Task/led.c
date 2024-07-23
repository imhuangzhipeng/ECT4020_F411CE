#include "gpio.h"

void led_reversed(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    return;
}
