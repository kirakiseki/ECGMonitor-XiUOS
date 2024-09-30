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
 * @file k210_systemreset.c
 * @brief k210 support reboot
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.06.27
 */


/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>

#include "riscv_internal.h"
#include "hardware/k210_memorymap.h"
 
typedef struct _k210_soft_reset
{
  uint32_t soft_reset : 1;
  uint32_t reserved : 31;
} __attribute__((packed, aligned(4))) k210_soft_reset_t;

volatile k210_soft_reset_t *const reset = (volatile k210_soft_reset_t *)(K210_SYSCTL_BASE + 0x30);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_systemreset
 *
 * Description:
 *   Internal reset logic.
 *
 ****************************************************************************/

void up_systemreset(void)
{
  reset->soft_reset = 1;

  /* Wait for the reset */

  for (; ; );
}
