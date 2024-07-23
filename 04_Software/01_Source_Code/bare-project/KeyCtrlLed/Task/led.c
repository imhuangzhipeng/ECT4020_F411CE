#include "gpio.h"

#define LED_Port GPIOA
#define LED_PIN GPIO_PIN_12

void led_reversed(void)
{
    HAL_GPIO_TogglePin(LED_Port, LED_PIN);
    return;
}
