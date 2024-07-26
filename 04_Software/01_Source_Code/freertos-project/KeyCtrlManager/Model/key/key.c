#include "key_device.h"
#include "gpio.h"

#define DEBOUNCE_TICKS 5 // 5ms

#define LONG_TICKS 2000 // 2s

#define RESET_DATA 0

typedef void (*pfkeyCallback)(void); // 绑定按键状态回调的函数指针

typedef struct
{
    /* 按键按下时间, 默认值为RESET_DATA */
    uint32_t key_press_tick;
    /* 按键状态类型, 默认NONE_PRESS */
    key_state_t key_state;
    /* 上一次读取的按键电平, 默认值为RESET_DATA, 表示没有读取按键电平 */
    uint8_t last_level;
    /* 按键按下时电平, 由初始化函数外部传入参数赋值 */
    uint8_t active_level;
} key_private_data_t;

typedef struct
{
    /* 获取当前的按键电平 */
    int8_t (*getKeyLevel)(void);
    /* 获取当前时钟滴答 */
    uint32_t (*getSysTick)(void);
    /* 按键状态回调函数的函数指针数组 */
    pfkeyCallback keyCallback[KEY_STATE_NUM];
} key_private_func_t;

typedef struct
{
    key_private_data_t data;
    key_private_func_t func;
} key_private_t;

/* 按键对象私有成员的数据域*/
static key_private_t __key_private_set[KEY_TYPE_NUM];

static int8_t keyInit(struct Key_Device *pDev,
                      uint8_t key_active_level,
                      int8_t (*pfgpio_read_pin)(void),
                      uint32_t (*pfget_system_tick)(void))
{
    key_private_t *key_private = pDev->priv_data;

    if ((NULL == key_private) &&
        (NULL == pfgpio_read_pin) &&
        (NULL == pfget_system_tick))
    {
        return -1;
    }

    /* 初始化key_device的私有变量 */
    key_private->data.key_press_tick = RESET_DATA;
    key_private->data.key_state = NONE_PRESS;
    key_private->data.last_level = RESET_DATA;
    key_private->data.active_level = key_active_level;

    /* 初始化key_device的私有方法 */
    key_private->func.getKeyLevel = pfgpio_read_pin;
    key_private->func.getSysTick = pfget_system_tick;

    return 0;
}

static int8_t keyBindingEvent(struct Key_Device *pDev,
                              key_state_t state,
                              void (*pfcallbackfunc)(void))
{
    key_private_t *key_private = pDev->priv_data;

    key_private->func.keyCallback[state] = pfcallbackfunc;

    if (NULL == key_private->func.keyCallback[state])
    {
        return -1;
    }

    return 0;
}

static int8_t keyScan(struct Key_Device *pDev)
{
    key_private_t *key_private = pDev->priv_data;

    uint8_t current_key_level = key_private->func.getKeyLevel();

    static uint32_t __tick = 0;

    // 当电平发生变化时
    if (current_key_level != key_private->data.last_level)
    {
        // 记录当前的系统时间
        __tick = key_private->func.getSysTick();
        // 更新当前的GPIO状态
        key_private->data.last_level = current_key_level;
    }

    // 如果tick不为0, 且当前系统时间tick与上一次读取的tick的差值大于消抖时间, 开启按键状态判断
    if ((__tick != 0) && ((key_private->func.getSysTick() - __tick) > DEBOUNCE_TICKS))
    {
        switch (key_private->data.key_state)
        {
        case NONE_PRESS:
        {
            // 如果当前电平与按键触发电平相同, 表示按键按下
            if (current_key_level == key_private->data.active_level)
            {
                // 更新key的按下时间
                key_private->data.key_press_tick = key_private->func.getSysTick();
                // 将data的key_event改为PRESS_DOWN
                key_private->data.key_state = PRESS_DOWN;
                // 执行PRESS_DOWN对应的回调函数
                key_private->func.keyCallback[PRESS_DOWN]();
            }
            else
            {
                __tick = 0;
            }
        }
        break;
        case PRESS_DOWN:
        {
            // 如果当前电平与按键触发电平不同, 表示按键抬起
            if (current_key_level != key_private->data.active_level)
            {
                __tick = 0;
                key_private->data.key_press_tick = key_private->func.getSysTick();
                key_private->data.key_state = PRESS_UP;
                key_private->func.keyCallback[PRESS_UP]();
            }
            // 当前系统时间tick与记录的按键按下时间差值大于长按时间阈值, 表示按键长按
            else if (key_private->func.getSysTick() - key_private->data.key_press_tick >= LONG_TICKS)
            {
                key_private->data.key_press_tick = key_private->func.getSysTick();
                key_private->data.key_state = PRESS_LONG;
                key_private->func.keyCallback[PRESS_LONG]();
            }
        }
        break;
        case PRESS_LONG:
        {
            // 如果当前电平与按键触发电平不同, 表示按键抬起
            if (current_key_level != key_private->data.active_level)
            {
                __tick = 0;
                key_private->data.key_state = PRESS_UP;
                key_private->func.keyCallback[PRESS_UP]();
            }
        }
        break;
        case PRESS_UP:
        {
            __tick = 0;
            key_private->data.key_state = NONE_PRESS;
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

struct Key_Device g_user_key = {
    .type = USER_KEY,
    .keyInit = keyInit,
    .keyBindingEvent = keyBindingEvent,
    .keyScan = keyScan,
    .priv_data = &__key_private_set[USER_KEY],
};
