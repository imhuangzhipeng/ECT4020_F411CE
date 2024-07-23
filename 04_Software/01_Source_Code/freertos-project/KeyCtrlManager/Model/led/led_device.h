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
    LED_RED_D3,
    LED_TYPE_MAX,
}LED_TYPE;

typedef enum
{
	OFF = 0,
	ON,
	Toggle
} LED_STATE;

typedef struct led_device_t
{
	LED_TYPE led_type;
	LED_STATE led_state;
	int (*ledInit)(struct led_device_t *pled_device);
	int (*ledControl)(struct led_device_t *pled_device, LED_STATE led_state);
	void (*setLedBrightness)(struct led_device_t *pled_device, int brightness);
	void (*setLedColor)(struct led_device_t *pled_device, int color);
	void *private_data;
} LedDevice, *pLedDevice;

/* Functions ----------------------------------------------------------------*/
pLedDevice getLedDevice(LED_TYPE led_type);

#endif /* __LED_DEVICE_H */
