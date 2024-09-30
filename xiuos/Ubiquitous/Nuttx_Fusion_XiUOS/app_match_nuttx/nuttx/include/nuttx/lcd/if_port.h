/****************************************************************************
 * if_port.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/**
* @file if_port.h
* @brief
* @version 1.0.0
* @author AIIT XUOS Lab
* @date 2022-07-21
*/

#ifndef _IF_PORT_H
#define _IF_PORT_H

#include <stdbool.h>
#include <debug.h>
#include <syslog.h>
#include "nuttx/lcd/lt768.h"
#include "nuttx/lcd/lt768_lib.h"

#define LCD_START_ADDR 0
#define STM32_FSMC_8 0

void lcd_backlight_init(bool enable);
void lcd_drv_init(void);
void LCD_CmdWrite(uint8_t cmd);
void LCD_DataWrite(uint8_t data);
uint8_t LCD_DataRead(void);
uint8_t LCD_StatusRead(void);
void LCD_DataWrite_Pixel(uint8_t data);

#endif
