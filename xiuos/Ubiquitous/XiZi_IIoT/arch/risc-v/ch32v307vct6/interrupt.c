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
#include <xs_assign.h>
#include <xs_base.h>
#include <xs_isr.h>
#include <xs_ktask.h>
#include "ch32v30x.h"
#include "cpuport.h"
#include "core_riscv.h"
#ifdef  TASK_ISOLATION
#include <xs_service.h>
#endif

extern x_ubase interrupt_from_sp;
extern x_ubase interrupt_to_sp;
extern x_ubase interrupt_new_task;

void sw_setpend(void)
{
    SysTick->CTLR |= (1<<31);
}

/*
 * clear soft interrupt
 */
void sw_clearpend(void)
{
    SysTick->CTLR &= ~(1<<31);
}

x_base DisableLocalInterrupt()
{
    x_base level;
    asm volatile ("csrrci %0, mstatus, 8" : "=r"(level));
    return level;
}

void EnableLocalInterrupt(x_base level)
{
    asm volatile ("csrw mstatus, %0" :: "r"(level));
}

int ArchEnableHwIrq(uint32_t irq_num)
{
    NVIC_SetPriority(irq_num, 1);
    NVIC_EnableIRQ(irq_num);
}

int ArchDisableHwIrq(uint32_t irq_num)
{
    NVIC_DisableIRQ(irq_num);
}

struct ExceptionStackFrame
{
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t x31;
};

void PrintStackFrame(uintptr_t * sp)
{
    struct ExceptionStackFrame * esf = (struct ExceptionStackFrame *)(sp+1);

    KPrintf("\n=================================================================\n");
    KPrintf("x1 (ra   : Return address                ) ==> 0x%08x%08x\n", esf->x1 >> 32  , esf->x1 & UINT32_MAX);
    KPrintf("x2 (sp   : Stack pointer                 ) ==> 0x%08x%08x\n", esf->x2 >> 32  , esf->x2 & UINT32_MAX);
    KPrintf("x3 (gp   : Global pointer                ) ==> 0x%08x%08x\n", esf->x3 >> 32  , esf->x3 & UINT32_MAX);
    KPrintf("x4 (tp   : Task pointer                  ) ==> 0x%08x%08x\n", esf->x4 >> 32  , esf->x4 & UINT32_MAX);
    KPrintf("x5 (t0   : Temporary                     ) ==> 0x%08x%08x\n", esf->x5 >> 32  , esf->x5 & UINT32_MAX);
    KPrintf("x6 (t1   : Temporary                     ) ==> 0x%08x%08x\n", esf->x6 >> 32  , esf->x6 & UINT32_MAX);
    KPrintf("x7 (t2   : Temporary                     ) ==> 0x%08x%08x\n", esf->x7 >> 32  , esf->x7 & UINT32_MAX);
    KPrintf("x8 (s0/fp: Save register,frame pointer   ) ==> 0x%08x%08x\n", esf->x8 >> 32  , esf->x8 & UINT32_MAX);
    KPrintf("x9 (s1   : Save register                 ) ==> 0x%08x%08x\n", esf->x9 >> 32  , esf->x9 & UINT32_MAX);
    KPrintf("x10(a0   : Function argument,return value) ==> 0x%08x%08x\n", esf->x10 >> 32 , esf->x10 & UINT32_MAX);
    KPrintf("x11(a1   : Function argument,return value) ==> 0x%08x%08x\n", esf->x11 >> 32 , esf->x11 & UINT32_MAX);
    KPrintf("x12(a2   : Function argument             ) ==> 0x%08x%08x\n", esf->x12 >> 32 , esf->x12 & UINT32_MAX);
    KPrintf("x13(a3   : Function argument             ) ==> 0x%08x%08x\n", esf->x13 >> 32 , esf->x13 & UINT32_MAX);
    KPrintf("x14(a4   : Function argument             ) ==> 0x%08x%08x\n", esf->x14 >> 32 , esf->x14 & UINT32_MAX);
    KPrintf("x15(a5   : Function argument             ) ==> 0x%08x%08x\n", esf->x15 >> 32 , esf->x15 & UINT32_MAX);
    KPrintf("x16(a6   : Function argument             ) ==> 0x%08x%08x\n", esf->x16 >> 32 , esf->x16 & UINT32_MAX);
    KPrintf("x17(a7   : Function argument             ) ==> 0x%08x%08x\n", esf->x17 >> 32 , esf->x17 & UINT32_MAX);
    KPrintf("x18(s2   : Save register                 ) ==> 0x%08x%08x\n", esf->x18 >> 32 , esf->x18 & UINT32_MAX);
    KPrintf("x19(s3   : Save register                 ) ==> 0x%08x%08x\n", esf->x19 >> 32 , esf->x19 & UINT32_MAX);
    KPrintf("x20(s4   : Save register                 ) ==> 0x%08x%08x\n", esf->x20 >> 32 , esf->x20 & UINT32_MAX);
    KPrintf("x21(s5   : Save register                 ) ==> 0x%08x%08x\n", esf->x21 >> 32 , esf->x21 & UINT32_MAX);
    KPrintf("x22(s6   : Save register                 ) ==> 0x%08x%08x\n", esf->x22 >> 32 , esf->x22 & UINT32_MAX);
    KPrintf("x23(s7   : Save register                 ) ==> 0x%08x%08x\n", esf->x23 >> 32 , esf->x23 & UINT32_MAX);
    KPrintf("x24(s8   : Save register                 ) ==> 0x%08x%08x\n", esf->x24 >> 32 , esf->x24 & UINT32_MAX);
    KPrintf("x25(s9   : Save register                 ) ==> 0x%08x%08x\n", esf->x25 >> 32 , esf->x25 & UINT32_MAX);
    KPrintf("x26(s10  : Save register                 ) ==> 0x%08x%08x\n", esf->x26 >> 32 , esf->x26 & UINT32_MAX);
    KPrintf("x27(s11  : Save register                 ) ==> 0x%08x%08x\n", esf->x27 >> 32 , esf->x27 & UINT32_MAX);
    KPrintf("x28(t3   : Temporary                     ) ==> 0x%08x%08x\n", esf->x28 >> 32 , esf->x28 & UINT32_MAX);
    KPrintf("x29(t4   : Temporary                     ) ==> 0x%08x%08x\n", esf->x29 >> 32 , esf->x29 & UINT32_MAX);
    KPrintf("x30(t5   : Temporary                     ) ==> 0x%08x%08x\n", esf->x30 >> 32 , esf->x30 & UINT32_MAX);
    KPrintf("x31(t6   : Temporary                     ) ==> 0x%08x%08x\n", esf->x31 >> 32 , esf->x31 & UINT32_MAX);
    KPrintf("=================================================================\n");
}

void HwInterruptcontextSwitch(x_ubase from_sp, x_ubase to_sp, struct TaskDescriptor* new_task, void* context) {
    interrupt_from_sp = (x_ubase)from_sp;
    interrupt_to_sp = (x_ubase)to_sp;
    interrupt_new_task = (x_ubase)new_task;
    sw_setpend();
}