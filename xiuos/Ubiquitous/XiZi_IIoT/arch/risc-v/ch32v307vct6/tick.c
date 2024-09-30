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

#include <xs_ktick.h>
#include <xs_isr.h>
#include <xs_assign.h>
#include "ch32v30x.h"
#include "ch32v30x_it.h"
#include "core_riscv.h"

extern void KTaskOsAssignAfterIrq(void *);

void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    x_base level;
    level = DisableLocalInterrupt();
    isrManager.done->incCounter();
    EnableLocalInterrupt(level);
    SysTick->SR = 0;
    TickAndTaskTimesliceUpdate();
    KTaskOsAssignAfterIrq(NONE);
    /* leave interrupt */
    level = DisableLocalInterrupt();
    isrManager.done->decCounter();
    EnableLocalInterrupt(level);
    FREE_INT_SP();
}