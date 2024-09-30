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

#include <stdint.h>
#include <board.h>
#include "gic.h"

#define ARCH_MAX_IRQ_NUM  PLATFORM_MAX_IRQ_NR

int32_t ArchEnableHwIrq(uint32_t irq_num, uint32_t cpu_id);
int32_t ArchDisableHwIrq(uint32_t irq_num, uint32_t cpu_id);

//! @brief
typedef enum {
    CPU_0,
    CPU_1,
    CPU_2,
    CPU_3,
} cpuid_e;

struct ExceptionStackRegister
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t r13_sp;
	uint32_t r14_lr;
	uint32_t r15_pc;
	uint32_t cpsr;
};

#endif
