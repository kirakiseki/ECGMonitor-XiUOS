/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file k210_leds.c
 * @brief xidatong-riscv64 k210_leds.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.06.08
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#include "k210_fpioa.h"
#include "k210_gpiohs.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void board_autoled_initialize(void)
{
}

void board_autoled_on(int led)
{
}

void board_autoled_off(int led)
{
}
