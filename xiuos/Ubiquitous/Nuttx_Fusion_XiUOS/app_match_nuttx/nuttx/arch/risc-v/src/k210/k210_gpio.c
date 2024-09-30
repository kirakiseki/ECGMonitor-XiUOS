/****************************************************************************
 * arch/risc-v/src/k210/k210_gpio.c
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
* @file k210_gpio.c
* @brief nuttx source code
*                https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-06-30
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <assert.h>
#include <debug.h>

#include "riscv_internal.h"
#include "k210_memorymap.h"
#include "k210_gpio.h"
#include "k210_fpioa.h"
#include "k210_sysctl.h"

#define GPIO_MAX_PINNO 8

/****************************************************************************
 * Private Function declaration
 ****************************************************************************/
static void set_bit(volatile uint32_t *bits, uint32_t mask, uint32_t value);
static void set_bit_offset(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value);
static void set_gpio_bit(volatile uint32_t *bits, size_t offset, uint32_t value);
static uint32_t get_bit(volatile uint32_t *bits, uint32_t mask, size_t offset);
static uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset);

/****************************************************************************
 * Private Data
 ****************************************************************************/
volatile gpio_t *const gpio = (volatile gpio_t *)K210_GPIO_BASE;

/****************************************************************************
 * Private Function definition
 ****************************************************************************/
static void set_bit(volatile uint32_t *bits, uint32_t mask, uint32_t value)
{
    uint32_t org = (*bits) & ~mask;
    *bits = org | (value & mask);
}

static void set_bit_offset(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value)
{
    set_bit(bits, mask << offset, value << offset);
}

static void set_gpio_bit(volatile uint32_t *bits, size_t offset, uint32_t value)
{
    set_bit_offset(bits, 1, offset, value);
}

static uint32_t get_bit(volatile uint32_t *bits, uint32_t mask, size_t offset)
{
    return ((*bits) & (mask << offset)) >> offset;
}

static uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset)
{
    return get_bit(bits, 1, offset);
}


/****************************************************************************
 * Public Function definition
 ****************************************************************************/
int gpio_init(void)
{
    return sysctl_clock_enable(SYSCTL_CLOCK_GPIO);
}

void gpio_set_drive_mode(uint8_t pin, gpio_drive_mode_t mode)
{
    DEBUGASSERT(pin < GPIO_MAX_PINNO);
    int io_number = fpioa_get_io_by_function(K210_IO_FUNC_GPIO0 + pin);
    DEBUGASSERT(io_number >= 0);

    fpioa_pull_t pull = FPIOA_PULL_NONE;
    uint32_t dir = 0;

    switch(mode)
    {
        case GPIO_DM_INPUT:
            pull = FPIOA_PULL_NONE;
            dir = 0;
            break;
        case GPIO_DM_INPUT_PULL_DOWN:
            pull = FPIOA_PULL_DOWN;
            dir = 0;
            break;
        case GPIO_DM_INPUT_PULL_UP:
            pull = FPIOA_PULL_UP;
            dir = 0;
            break;
        case GPIO_DM_OUTPUT:
            pull = FPIOA_PULL_DOWN;
            dir = 1;
            break;
        default:
            break;
    }

    fpioa_set_io_pull(io_number, pull);
    set_gpio_bit(gpio->direction.u32, pin, dir);
}

gpio_pin_value_t gpio_get_pin(uint8_t pin)
{
    DEBUGASSERT(pin < GPIO_MAX_PINNO);
    uint32_t dir = get_gpio_bit(gpio->direction.u32, pin);
    volatile uint32_t *reg = dir ? gpio->data_output.u32 : gpio->data_input.u32;
    return get_gpio_bit(reg, pin);
}

void gpio_set_pin(uint8_t pin, gpio_pin_value_t value)
{
    DEBUGASSERT(pin < GPIO_MAX_PINNO);
    uint32_t dir = get_gpio_bit(gpio->direction.u32, pin);
    volatile uint32_t *reg = dir ? gpio->data_output.u32 : gpio->data_input.u32;
    DEBUGASSERT(dir == 1);
    set_gpio_bit(reg, pin, value);
}
