/****************************************************************************
 * arch/risc-v/src/k210/k210_gpiohs.h
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
* @file k210_gpiohs.h
* @brief nuttx source code
*                https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-03-23
*/

#ifndef __ARCH_RISCV_SRC_K210_K210_GPIOHS_H
#define __ARCH_RISCV_SRC_K210_K210_GPIOHS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "k210_gpio_common.h"

#define HS_GPIO(n) (K210_IO_FUNC_GPIOHS0 + n)

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: k210_gpiohs_set_direction
 *
 * Description:
 *   Set gpiohs direction
 *
 * Input Parameters:
 *   io  - IO number
 *   dir - true for output, false for input
 *
 ****************************************************************************/

void k210_gpiohs_set_direction(uint32_t io, gpio_drive_mode_t mode);

/****************************************************************************
 * Name: k210_gpiohs_set_value
 *
 * Description:
 *   Set gpiohs direction
 *
 * Input Parameters:
 *   io  - IO number
 *   dir - true for high level, false for low level
 *
 ****************************************************************************/

void k210_gpiohs_set_value(uint32_t io, bool val);

/****************************************************************************
 * Name: k210_gpiohs_get_value
 *
 * Description:
 *   Get gpiohs level
 *
 * Input Parameters:
 *   io  - IO number
 *
 * Returned Value:
 *   true for high level, false for low level
 *
 ****************************************************************************/

bool k210_gpiohs_get_value(uint32_t io);

#endif /* __ARCH_RISCV_SRC_K210_K210_GPIOHS_H */
