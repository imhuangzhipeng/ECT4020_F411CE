#include "gpio.h"

int key_is_pressed(void)
{
    for (int i = 0; i < 10; i++)
    {
        GPIO_PinState pinState = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
				HAL_Delay(1);

        if (GPIO_PIN_RESET != pinState)
            return 0;
    }

    while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0){}

    return 1;
}
