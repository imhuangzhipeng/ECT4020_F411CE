/**
  * @file    device_linklist.h
  * @brief   Circular Linked List for devices
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
#ifndef __DEVICE_LINKLIST_H
#define __DEVICE_LINKLIST_H

/* Files includes  -----------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/
#define container_of(ptr, type, member) \
    ((type *)((char *)ptr - (unsigned int)&((type *)0)->member))

/* Variables -----------------------------------------------------------------*/
typedef struct device_node_t
{
    struct device_node_t *pnext;
} DeviceNode,*pDeviceNode;

/* Functions ----------------------------------------------------------------*/
void addDevLinkList(pDeviceNode *list, DeviceNode *item);

#endif /* __DEVICE_LINKLIST_H */
