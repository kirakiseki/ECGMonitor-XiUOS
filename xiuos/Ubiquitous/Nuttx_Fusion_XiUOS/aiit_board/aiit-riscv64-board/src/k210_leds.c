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
 * @brief aiit-riscv64-board k210_leds.c
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
  k210_fpioa_config(BOARD_LED_PAD, BOARD_LED_IO_FUNC | K210_IOFLAG_GPIOHS);
  k210_gpiohs_set_direction(BOARD_LED_IO, true);
  k210_gpiohs_set_value(BOARD_LED_IO, true); /* LED off */
}

void board_autoled_on(int led)
{
  if (led == LED_PANIC)
    {
      k210_gpiohs_set_value(BOARD_LED_IO, false);
    }
}

void board_autoled_off(int led)
{
  if (led == LED_PANIC)
    {
      k210_gpiohs_set_value(BOARD_LED_IO, true);
    }
}
