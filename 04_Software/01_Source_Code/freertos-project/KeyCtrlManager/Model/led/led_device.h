/**
  * @file    led_device.h
  * @brief   led device interface
  * 
  ******************************************************************************
  * @version 1.0.0
  * @date    2024/07/16
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
#ifndef __LED_DEVICE_H
#define __LED_DEVICE_H

/* Files includes  -----------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef enum
{
  LED_BLUE_D2,
  // LED_RED_D3,
  LED_TYPE_NUM,
} led_type_t;

typedef enum
{
  OFF = 0,
  ON,
  Toggle
} led_state_t;

struct LED_Device
{
  led_type_t type;
  led_state_t state;
  int (*ledInit)(struct LED_Device *pDev);
  int (*ledControl)(struct LED_Device *pDev, led_state_t state);
  void (*setLedBrightness)(struct LED_Device *pDev, int brightness);
  void (*setLedColor)(struct LED_Device *pDev, int color);
  void *priv_data;
};

/* Functions ----------------------------------------------------------------*/
struct LED_Device *getLedDevice(led_type_t type);

#endif /* __LED_DEVICE_H */
