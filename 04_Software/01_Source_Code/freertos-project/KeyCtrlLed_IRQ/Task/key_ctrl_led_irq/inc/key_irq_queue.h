/**
  * @file    key_irq_queue.h
  * @brief   XXXXXXX
  * 
  ******************************************************************************
  * @version 1.0.0
  * @date    2024/07/23
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
#ifndef __KEY_IRQ_QUEUE_H
#define __KEY_IRQ_QUEUE_H

/* Files includes  -----------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef enum
{
    KEY_NULL,
    KEY_PRESS,
    KEY_RELEASE,
} key_state_t;

typedef enum
{
    KEY_NO_PRESS,
    KEY_SHORT_PRESS,
    KEY_LONG_PRESS,
    KEY_DOUBLE_PRESS,
} key_result_t;

typedef struct key_interface_t
{
    void (*pfinit)(void);
    void (*pftask)(void);
} KeyInterface;

/* Functions ----------------------------------------------------------------*/


#endif /* __KEY_IRQ_QUEUE_H */
