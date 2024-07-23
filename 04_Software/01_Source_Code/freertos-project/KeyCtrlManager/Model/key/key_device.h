/**
  * @file    key_device.h
  * @brief   key device interface
  * 
  ******************************************************************************
  * @version 1.0.0
  * @date    2024/07/17
  * @author  Chippen Huang 
  * 
  *******************************************************************************
  * @attention 
  * 
  * Copyright (C), 2023-2024, XXXX company. 
  * All rights reserved. 
  * 
  * This software is licensed under terms that can be found in the LICENSE file 
  * in the root directory of this software component. 
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __KEY_DEVICE_H
#define __KEY_DEVICE_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define PRESS_LONG_TICK 5

/* Variables -----------------------------------------------------------------*/
typedef void (*KeyCallBackFun)(void);

typedef enum
{
    // 无按键
    NONE_PRESS,
    // 按下
    PRESS_DOWN,
    // 抬起
    PRESS_UP,
    // 长按
    PRESS_LONG,
    // 按键事件类型数量
    KYE_TYPE_NUM,
} KEY_EVENT;

typedef enum
{
    USER_KEY,
    BOOT_KEY_SW2,
    RESET_KEY_SW3,
    KEY_NUM_MAX,
} KEY_TYPE;

typedef struct
{
    uint32_t (*get_system_tick)(void);
} system_tick_t;

typedef struct key_device_t
{
    KEY_TYPE key_type;

    int8_t (*keyInit)(struct key_device_t *key_device,
                      uint8_t active_level,
                      int8_t (*pfgpio_read_pin)(void),
                      uint32_t (*get_system_tick)(void));

    int8_t (*keyBindingEvent)(struct key_device_t *key_device,
                              KEY_EVENT key_event,
                              KeyCallBackFun pfcallbackfun);

    int8_t (*keyScan)(struct key_device_t *key_device);

    void *private_data;
} KeyDevice, *pKeyDevice;

/* Functions ----------------------------------------------------------------*/
pKeyDevice getKeyDevice(KEY_TYPE key_type);

#endif /* __KEY_DEVICE_H */
