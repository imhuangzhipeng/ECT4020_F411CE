/**
  * @file    user_rtt_printf.h
  * @brief   XXXXXXX
  * 
  ******************************************************************************
  * @version 1.0.0
  * @date    2024/08/02
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
#ifndef __USER_RTT_PRINTF_H
#define __USER_RTT_PRINTF_H

/* Files includes  -----------------------------------------------------------*/
#include "SEGGER_RTT.h"

/* Defines -------------------------------------------------------------------*/
#define LOG_DEBUG 1

#if LOG_DEBUG

#define LOG_PROTO(type, color, format, ...)        \
    SEGGER_RTT_printf(0, "%s%s" format "\r\n%s", \
                      color,                       \
                      type,                        \
                      ##__VA_ARGS__,               \
                      RTT_CTRL_RESET)

/* 清屏*/
#define LOG_CLEAR() SEGGER_RTT_WriteString(0, "  " RTT_CTRL_CLEAR)

/* 无颜色日志输出 */
#define LOG(format, ...) LOG_PROTO("", "", format, ##__VA_ARGS__)

/* 有颜色格式日志输出 */
#define LOGI(format, ...) LOG_PROTO("I: ", RTT_CTRL_TEXT_BRIGHT_GREEN, format, ##__VA_ARGS__)
#define LOGW(format, ...) LOG_PROTO("W: ", RTT_CTRL_TEXT_BRIGHT_YELLOW, format, ##__VA_ARGS__)
#define LOGE(format, ...) LOG_PROTO("E: ", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)

#else /* LOG_DEBUG */
#define LOG_CLEAR()
#define LOG
#define LOGI
#define LOGW
#define LOGE

#endif /* LOG_DEBUG */

/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/


#endif /* __USER_RTT_PRINTF_H */
