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

#define ARCH_MAX_IRQ_NUM (256)

//#define ARCH_IRQ_NUM_OFFSET 0

//#define SYSTICK_IRQN       15
//#define UART1_IRQn         36
//#define UART2_IRQn         37

#define ARCH_IRQ_NUM_OFFSET 16

#define SYSTICK_IRQN       -1
#define UART1_IRQn         20
#define UART2_IRQn         21

#define USB1_IRQn          113
#define USB2_IRQn          112

int32 ArchEnableHwIrq(uint32 irq_num);
int32 ArchDisableHwIrq(uint32 irq_num);

#endif
