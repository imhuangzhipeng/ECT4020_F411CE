/**
  * @file    mcu_gpio_driver.h
  * @brief   This file is the GPIO base class for STM32MCU
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
#ifndef __MCU_GPIO_DRIVER_H
#define __MCU_GPIO_DRIVER_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
typedef enum
{
  LOW_LEVEL = 0u, // 低电平
  HIGH_LEVEL      // 高电平
} gpio_pinstate;

typedef struct gpio_interface_t
{
  void (*pfgpio_init)(void);                                                            // 初始化函数
  void (*pfgpio_write_pin)(void *gpio_port, uint16_t gpio_pin, gpio_pinstate pinstate); // 写入引脚状态函数
  int8_t (*pfgpio_read_pin)(void *gpio_port, uint16_t gpio_pin);                        // 读取引脚状态函数
  void (*pfgpio_toggle_pin)(void *gpio_port, uint16_t gpio_pin);                        // 翻转引脚状态函数
} gpio_interface_t, *pgpio_interface_t;

/* Functions ----------------------------------------------------------------*/
int8_t register_gpio_function(void (*pfgpio_init)(void),
                              void (*pfgpio_write_pin)(void *gpio_port, uint16_t gpio_pin, gpio_pinstate pinstate),
                              int8_t (*pfgpio_read_pin)(void *gpio_port, uint16_t gpio_pin),
                              void (*pfgpio_toggle_pin)(void *gpio_port, uint16_t gpio_pin));

pgpio_interface_t get_gpio_handle(void);

#endif /* __MCU_GPIO_DRIVER_H */
