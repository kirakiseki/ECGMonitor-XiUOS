/*
* Copyright (c) 2022 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file k210_touch.h
 * @brief gt911 touch driver
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.10.25
 */

#ifndef _K210_TOUCH_H_
#define _K210_TOUCH_H_

#include <nuttx/config.h>
#include <nuttx/kmalloc.h>
#include <nuttx/board.h>
#include <arch/board/board.h>
#include <nuttx/time.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <debug.h>
#include <assert.h>
#include <nuttx/time.h>
#include <nuttx/fs/fs.h>
#include "k210_config.h"
#include "k210_fpioa.h"
#include "k210_gpiohs.h"
#include "nuttx/arch.h"
#include "k210_gpio_common.h"

#define GT911_FUNC_GPIO(n)  ((K210_IO_FUNC_GPIOHS0 + n) | K210_IOFLAG_GPIOHS)

#define GT911_MAX_WIDTH		        (uint16_t)800 
#define GT911_MAX_HEIGHT	        (uint16_t)480
#define CT_CMD_WR	                (uint8_t)0XBA
#define CT_CMD_RD                   (uint8_t)0XBB
#define CT_MAX_TOUCH                (uint8_t)5
#define GT911_COMMAND_REG   		(uint16_t)0x8040
#define GT911_CONFIG_REG			(uint16_t)0x8047
#define GT911_PRODUCT_ID_REG 		(uint16_t)0x8140
#define GT911_FIRMWARE_VERSION_REG  (uint16_t)0x8144
#define GT911_READ_XY_REG 			(uint16_t)0x814E

typedef struct
{
	uint8_t TouchCount;
	uint8_t Touchkeytrackid[CT_MAX_TOUCH];
	uint16_t X[CT_MAX_TOUCH];
	uint16_t Y[CT_MAX_TOUCH];
	uint16_t S[CT_MAX_TOUCH];
}GT911_Dev;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t press;
}POINT;

void board_touch_initialize(void);

#endif
