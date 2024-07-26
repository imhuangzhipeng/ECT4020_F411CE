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
typedef enum
{
    USER_KEY,
    // BOOT_KEY_SW2,
    // RESET_KEY_SW3,
    KEY_TYPE_NUM,
} key_type_t;

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
    // 按键状态
    KEY_STATE_NUM,
} key_state_t;

struct Key_Device
{
    key_type_t type;

    int8_t (*keyInit)(struct Key_Device *pDev,
                      uint8_t key_active_level,
                      int8_t (*pfgpio_read_pin)(void),
                      uint32_t (*pfget_system_tick)(void));

    int8_t (*keyBindingEvent)(struct Key_Device *pDev,
                              key_state_t state,
                              void (*pfcallbackfunc)(void));

    int8_t (*keyScan)(struct Key_Device *pDev);

    void *priv_data;
};

/* Functions ----------------------------------------------------------------*/
struct Key_Device *getKeyDevice(key_type_t type);

#endif /* __KEY_DEVICE_H */
