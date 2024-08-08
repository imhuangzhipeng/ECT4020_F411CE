/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <elog.h>
#include "SEGGER_RTT.h"
#include "stm32f4xx_hal.h"

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    
    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
    /* add your code here */
    SEGGER_RTT_Write(0, log, size);
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    __disable_irq();
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    __enable_irq();
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {

    /* add your code here */
    uint32_t ticks = HAL_GetTick();
    uint32_t seconds = ticks / 1000; // 假设每秒1000个tick
    uint32_t milliseconds = ticks % 1000;

    // 创建一个足够大的缓冲区来存储时间字符串
    static char time_str[20] = {0}; // 20字符足够存放 "00:00:00.000"

    // 使用 snprintf 来格式化时间字符串
    snprintf(time_str, sizeof(time_str), "%02u:%02u:%02u.%03u",
             seconds / 3600, (seconds % 3600) / 60, seconds % 60, milliseconds);

    return time_str;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "process:";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    return "thread:";
}
