/**
  * @file    uart_printf.h
  * @brief   UART formatted output
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
#ifndef __UART_PRINTF_H
#define __UART_PRINTF_H

/* Files includes  -----------------------------------------------------------*/
#include <stdio.h>

/* Defines -------------------------------------------------------------------*/
#define osPrintf(...) do{ taskENTER_CRITICAL();\
                          printf(__VA_ARGS__);\
                          taskEXIT_CRITICAL();\
                        }while(0)

/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/

#endif /* __UART_PRINTF_H */
