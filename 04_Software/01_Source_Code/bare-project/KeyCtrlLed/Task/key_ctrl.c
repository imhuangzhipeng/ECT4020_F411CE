#include <stdio.h>

extern int key_is_pressed(void);
extern void led_reversed(void);

void led_key_ctrl(void)
{
    // key is pressed?
    if (key_is_pressed())
    {
				printf("key pressed!\r\n");
        led_reversed();
    }
    
    return;
}
