/**
  * @file    uart_device.h
  * @brief   UART device interface
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
#ifndef __UART_DEVICE_H
#define __UART_DEVICE_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
struct UART_Device
{
  char *name;
  int (*Init)(struct UART_Device *pDev, int baud, char parity, int data_bit, int stop_bit);
  int (*Send)(struct UART_Device *pDev, uint8_t *datas, uint32_t len, int timeout);
  int (*RecvByte)(struct UART_Device *pDev, uint8_t *data, int timeout);
  void *priv_data;
};

/* Functions ----------------------------------------------------------------*/
struct UART_Device *getUARTDevice(char *name);

#endif /* __UART_DEVICE_H */
