#include "key_device.h"
#include "gpio.h"

#define DEBOUNCE_TICKS 5 // 5ms

#define LONG_TICKS 2000 // 2s

typedef struct
{
    // 按键消抖时间
    uint32_t debounce_tick;
    // 按键按下时间
    uint32_t key_tick;
    // 按键事件类型
    KEY_EVENT key_event;
    // 当前的按键电平
    uint8_t now_level;
    // 按键有效电平
    uint8_t active_level;
    // 读取GPIO引脚状态
    int8_t (*pfgpio_read_pin)(void);
    // 根据按键类型调用对应回调函数
    KeyCallBackFun pfkey_callback[KYE_TYPE_NUM];
    // 获取系统tick数
    system_tick_t system_tick;
} key_data_t;

/* 按键对象私有属性数据域*/
static key_data_t __g_key_priv_data_set[KEY_NUM_MAX];

static int8_t keyInit(struct key_device_t *key_device,
                      uint8_t active_level,
                      int8_t (*pfgpio_read_pin)(void),
                      uint32_t (*get_system_tick)(void))
{
    key_data_t *data = key_device->private_data;

    if ((NULL == data) &&
        (NULL == pfgpio_read_pin) &&
        (NULL == get_system_tick))
    {
        return -1;
    }

    data->active_level = active_level;
    data->pfgpio_read_pin = pfgpio_read_pin;
    data->system_tick.get_system_tick = get_system_tick;

    data->key_tick = 0;
    data->debounce_tick = 0;
    data->key_event = NONE_PRESS;
    data->now_level = data->pfgpio_read_pin();

    return 0;
}

static int8_t keyBindingEvent(struct key_device_t *key_device,
                              KEY_EVENT key_event,
                              KeyCallBackFun pfcallbackfun)
{
    key_data_t *data = key_device->private_data;

    data->pfkey_callback[key_event] = pfcallbackfun;

    if (NULL == data->pfkey_callback[key_event])
    {
        return -1;
    }

    return 0;
}

static int8_t keyScan(struct key_device_t *key_device)
{
    key_data_t *data = key_device->private_data;

    uint8_t read_level = data->pfgpio_read_pin();

    static uint32_t __tick = 0;

    if (read_level != data->now_level)
    {
        // 记录当前的系统时间
        __tick = data->system_tick.get_system_tick();
        // 更新当前的GPIO状态
        data->now_level = read_level;
    }

    // 如果tick不为0, 且当前系统时间tick与上一次读取的tick的差值大于debounce_ticks, 开启按键事件判断
    if ((__tick != 0) && ((data->system_tick.get_system_tick() - __tick) > DEBOUNCE_TICKS))
    {
        switch (data->key_event)
        {
        case NONE_PRESS:
        {
            // 如果当前读取的level与data的active_level相同, 表示按键按下
            if (read_level == data->active_level)
            {
                // 记录当前系统时间tick
                data->key_tick = data->system_tick.get_system_tick();
                // 将data的key_event改为PRESS_DOWN
                data->key_event = PRESS_DOWN;
                // 执行PRESS_DOWN对应的回调函数
                data->pfkey_callback[PRESS_DOWN]();
            }
            else
            {
                __tick = 0;
            }
        }
        break;
        case PRESS_DOWN:
        {
            // 如果当前读取的level与data的active_level不同, 表示按键抬起
            if (read_level != data->active_level)
            {
                __tick = 0;
                data->key_tick = data->system_tick.get_system_tick();
                data->key_event = PRESS_UP;
                data->pfkey_callback[PRESS_UP]();
            }
            // 如果当前系统时间tick与data的keytick的差值大于LONG_TICKS, 表示按键长按
            else if (data->system_tick.get_system_tick() - data->key_tick >= LONG_TICKS)
            {
                data->key_tick = data->system_tick.get_system_tick();
                data->key_event = PRESS_LONG;
                data->pfkey_callback[PRESS_LONG]();
            }
        }
        break;
        case PRESS_LONG:
        {
            // 如果当前读取的level与data的active_level不同, 表示按键抬起
            if (read_level != data->active_level)
            {
                __tick = 0;
                data->key_event = PRESS_UP;
                data->pfkey_callback[PRESS_UP]();
            }
        }
        break;
        case PRESS_UP:
        {
            __tick = 0;
            data->key_event = NONE_PRESS;
        }
        break;
        default:
        {
            return -1;
        }
        }
    }

    return 0;
}

KeyDevice g_user_key = {
    .key_type = USER_KEY,
    .keyInit = keyInit,
    .keyBindingEvent = keyBindingEvent,
    .keyScan = keyScan,
    .private_data = &__g_key_priv_data_set[USER_KEY],
};
