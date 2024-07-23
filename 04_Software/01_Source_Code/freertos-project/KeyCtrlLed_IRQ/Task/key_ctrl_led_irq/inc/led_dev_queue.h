/**
  * @file    led_dev_queue.h
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
#ifndef __LED_DEV_QUEUE_H
#define __LED_DEV_QUEUE_H

/* Files includes  -----------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef struct led_interface_t
{
    void (*pfinit)(void);
    void (*pftask)(void);
    void (*pfon)(void);
    void (*pfoff)(void);
    void (*pftoggle)(void);
} LedInterface;

/* Functions ----------------------------------------------------------------*/


#endif /* __LED_DEV_QUEUE_H */
