/**
  * @file    led_task_queue.h
  * @brief   XXXXXXX
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
#ifndef __LED_TASK_QUEUE_H
#define __LED_TASK_QUEUE_H

/* Files includes  -----------------------------------------------------------*/
#include "led_device.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef enum
{
  LED_UNDO = 0,
  LED_TOGGLE,
  LED_FLASH,
  LED_PWM,
}MANAGE_LED_CTRL;

typedef struct led_ctrl_ope_t
{
  LED_TYPE led_device;
  MANAGE_LED_CTRL led_ctrl;
}LedCtrlOpe;

/* Functions ----------------------------------------------------------------*/

#endif /* __LED_TASK_QUEUE_H */
