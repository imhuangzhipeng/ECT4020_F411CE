/**
  * @file    key_task_queue.h
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
#ifndef __KEY_TASK_QUEUE_H
#define __KEY_TASK_QUEUE_H

/* Files includes  -----------------------------------------------------------*/
#include "key_device.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef enum
{ 
  NONE_EVENT = 0,
  KEY_PRESS_EVENT,
  KEY_LONG_EVENT,
}MANAGE_KEY_EVENT;

typedef struct key_ctrl_ope_t
{
  KEY_TYPE key_device;
  MANAGE_KEY_EVENT key_event;
}KeyCtrlOpe;

/* Functions ----------------------------------------------------------------*/

#endif /* __KEY_TASK_QUEUE_H */
