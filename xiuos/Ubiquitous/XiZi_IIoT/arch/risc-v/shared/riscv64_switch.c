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

#include "register_para.h"
#include <xs_base.h>
#include <xs_ktask.h>

void __attribute__((naked)) SwitchKTaskContextExit()
{
    asm volatile (LoadDS " a0,   0 * " RegLengthS "(sp)");
    asm volatile ("csrw mepc, a0");
#ifdef  TASK_ISOLATION
    asm volatile (LoadDS " a0,   2 * " RegLengthS "(sp)");
    asm volatile ("csrw mstatus, a0");
    asm volatile ("mv a0, sp");
    asm volatile ("jal RestoreMstatus");
#else
    asm volatile ("li    t0, 0x00007800");
    asm volatile ("csrw  mstatus, t0");
    asm volatile (LoadDS " a0,   2 * " RegLengthS "(sp)");
    asm volatile ("csrs mstatus, a0");
#endif
    asm volatile (LoadDS " ra,   1 * " RegLengthS "(sp)");
    asm volatile (LoadDS " tp,   4 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t0,   5 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t1,   6 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t2,   7 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s0,   8 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s1,   9 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a0,  10 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a1,  11 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a2,  12 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a3,  13 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a4,  14 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a5,  15 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a6,  16 * " RegLengthS "(sp)");
    asm volatile (LoadDS " a7,  17 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s2,  18 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s3,  19 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s4,  20 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s5,  21 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s6,  22 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s7,  23 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s8,  24 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s9,  25 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s10, 26 * " RegLengthS "(sp)");
    asm volatile (LoadDS " s11, 27 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t3,  28 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t4,  29 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t5,  30 * " RegLengthS "(sp)");
    asm volatile (LoadDS " t6,  31 * " RegLengthS "(sp)");
    asm volatile ("addi sp,  sp, 32 * " RegLengthS "");

#ifdef ARCH_RISCV_FPU
    asm volatile (FLOAD  " f0, 0 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f1, 1 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f2, 2 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f3, 3 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f4, 4 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f5, 5 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f6, 6 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f7, 7 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f8, 8 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f9, 9 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f10, 10 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f11, 11 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f12, 12 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f13, 13 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f14, 14 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f15, 15 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f16, 16 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f17, 17 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f18, 18 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f19, 19 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f20, 20 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f21, 21 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f22, 22 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f23, 23 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f24, 24 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f25, 25 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f26, 26 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f27, 27 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f28, 28 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f29, 29 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f30, 30 *" FREGBYTES "(sp)");
    asm volatile (FLOAD  " f31, 31 *" FREGBYTES "(sp)");
    asm volatile ("addi sp, sp, 32 *" FREGBYTES);
#endif

    asm volatile ("mret");
}

void __attribute__((naked)) SaveMpie()
{
    
#ifdef ARCH_RISCV_FPV
    asm volatile ("addi sp, sp, -32 *" FREGBYTES);

    asm volatile (FSTORE  " f0, 0 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f1, 1 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f2, 2 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f3, 3 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f4, 4 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f5, 5 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f6, 6 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f7, 7 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f8, 8 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f9, 9 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f10, 10 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f11, 11 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f12, 12 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f13, 13 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f14, 14 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f15, 15 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f16, 16 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f17, 17 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f18, 18 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f19, 19 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f20, 20 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f21, 21 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f22, 22 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f23, 23 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f24, 24 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f25, 25 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f26, 26 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f27, 27 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f28, 28 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f29, 29 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f30, 30 *" FREGBYTES "(sp)");
    asm volatile (FSTORE  " f31, 31 *" FREGBYTES "(sp)");
#endif

    asm volatile (StoreDS " a0,   2 * " RegLengthS "(sp)");
    asm volatile (StoreDS " tp,   4 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t0,   5 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t1,   6 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t2,   7 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s0,   8 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s1,   9 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a0,  10 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a1,  11 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a2,  12 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a3,  13 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a4,  14 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a5,  15 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a6,  16 * " RegLengthS "(sp)");
    asm volatile (StoreDS " a7,  17 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s2,  18 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s3,  19 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s4,  20 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s5,  21 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s6,  22 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s7,  23 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s8,  24 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s9,  25 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s10, 26 * " RegLengthS "(sp)");
    asm volatile (StoreDS " s11, 27 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t3,  28 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t4,  29 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t5,  30 * " RegLengthS "(sp)");
    asm volatile (StoreDS " t6,  31 * " RegLengthS "(sp)");
    asm volatile (LoadDS " sp,  (a1)");
    asm volatile ("mv   a0,   a2");
    asm volatile ("jal  RestoreCpusLockStatus");
    asm volatile ("j SwitchKTaskContextExit");
}

void __attribute__((naked)) SwitchKtaskContextTo(x_ubase to, struct TaskDescriptor *to_task)
{
    asm volatile (LoadDS " sp, (a0)\n");
    asm volatile ("mv a0, a1");
    asm volatile ("jal RestoreCpusLockStatus");
    asm volatile (LoadDS " a7, 2 * " RegLengthS "(sp)");
    asm volatile ("csrw mstatus, a7");
    asm volatile ("j SwitchKTaskContextExit");
}

void __attribute__((naked)) SwitchKtaskContext(x_ubase from, x_ubase to, struct TaskDescriptor *to_task)
{
    asm volatile ("addi  sp,  sp, -32 * " RegLengthS);
    asm volatile (StoreDS " sp,  (a0)");
    asm volatile (StoreDS " x1,   0 * " RegLengthS "(sp)");
    asm volatile (StoreDS " x1,   1 * " RegLengthS "(sp)");
    asm volatile ("csrr a0, mstatus");
#ifndef  TASK_ISOLATION
    asm volatile ("andi a0, a0, 8");
    asm volatile ("beqz a0, SaveMpie");
    asm volatile ("li   a0, 0x80");
#endif
    asm volatile ("j SaveMpie");
}

void __attribute__((naked)) HwInterruptcontextSwitch(x_ubase from, x_ubase to, struct TaskDescriptor *to_task, void *context)
{
    asm volatile (StoreDS " a3, 0(a0)");
    asm volatile (LoadDS  " sp, 0(a1)");
    asm volatile ("move  a0, a2");
    asm volatile ("call RestoreCpusLockStatus");
    asm volatile ("j SwitchKTaskContextExit");
}
