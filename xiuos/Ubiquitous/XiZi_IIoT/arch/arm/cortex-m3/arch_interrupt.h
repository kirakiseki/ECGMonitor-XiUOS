/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

#ifndef ARCH_INTERRUPT_H__
#define ARCH_INTERRUPT_H__

#include <xs_base.h>

#ifdef BOARD_CORTEX_M3_EVB
#define ARCH_MAX_IRQ_NUM (256)
#endif

#ifdef BOARD_STM32F103_NANO
#define ARCH_MAX_IRQ_NUM (64)
#endif

#define ARCH_IRQ_NUM_OFFSET 0

#ifdef BOARD_CORTEX_M3_EVB
#define SYSTICK_IRQN       15
#define UART1_IRQn         21
#endif

int32 ArchEnableHwIrq(uint32 irq_num);
int32 ArchDisableHwIrq(uint32 irq_num);

#endif
