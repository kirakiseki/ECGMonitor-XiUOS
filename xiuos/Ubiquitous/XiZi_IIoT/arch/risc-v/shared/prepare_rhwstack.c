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
#include <board.h>
#include <xs_base.h>
#include <xs_ktask.h>
#include <xs_isr.h>
#ifdef  TASK_ISOLATION
#include "encoding.h"
#include <stdint.h>
#include <xs_isolation.h>
#endif

struct StackRegisterContext
{
    x_ubase epc;        
    x_ubase ra;         
    x_ubase mstatus;    
    x_ubase gp;         
    x_ubase tp;         
    x_ubase t0;         
    x_ubase t1;         
    x_ubase t2;         
    x_ubase s0_fp;      
    x_ubase s1;         
    x_ubase a0;         
    x_ubase a1;       
    x_ubase a2;         
    x_ubase a3;         
    x_ubase a4;         
    x_ubase a5;         
    x_ubase a6;         
    x_ubase a7;         
    x_ubase s2;         
    x_ubase s3;        
    x_ubase s4;         
    x_ubase s5;         
    x_ubase s6;         
    x_ubase s7;       
    x_ubase s8;        
    x_ubase s9;       
    x_ubase s10;      
    x_ubase s11;        
    x_ubase t3;       
    x_ubase t4;     
    x_ubase t5;         
    x_ubase t6;

/* float register */
#ifdef ARCH_RISCV_FPU
    x_ubase f0;      /* f0  */
    x_ubase f1;      /* f1  */
    x_ubase f2;      /* f2  */
    x_ubase f3;      /* f3  */
    x_ubase f4;      /* f4  */
    x_ubase f5;      /* f5  */
    x_ubase f6;      /* f6  */
    x_ubase f7;      /* f7  */
    x_ubase f8;      /* f8  */
    x_ubase f9;      /* f9  */
    x_ubase f10;     /* f10 */
    x_ubase f11;     /* f11 */
    x_ubase f12;     /* f12 */
    x_ubase f13;     /* f13 */
    x_ubase f14;     /* f14 */
    x_ubase f15;     /* f15 */
    x_ubase f16;     /* f16 */
    x_ubase f17;     /* f17 */
    x_ubase f18;     /* f18 */
    x_ubase f19;     /* f19 */
    x_ubase f20;     /* f20 */
    x_ubase f21;     /* f21 */
    x_ubase f22;     /* f22 */
    x_ubase f23;     /* f23 */
    x_ubase f24;     /* f24 */
    x_ubase f25;     /* f25 */
    x_ubase f26;     /* f26 */
    x_ubase f27;     /* f27 */
    x_ubase f28;     /* f28 */
    x_ubase f29;     /* f29 */
    x_ubase f30;     /* f30 */
    x_ubase f31;     /* f31 */
#endif
};

uint8 KTaskStackSetup(struct TaskDescriptor *task)
{
    struct StackRegisterContext *stack_contex;
    int  i;

    task->stack_point  = (uint8 *)ALIGN_MEN_DOWN((x_ubase)(task->task_base_info.stack_start + task->task_base_info.stack_depth), RegLength);

    task->stack_point -= sizeof(struct StackRegisterContext);
    
    stack_contex = (struct StackRegisterContext *)task->stack_point;

    for (i = 0; i < sizeof(struct StackRegisterContext) / sizeof(x_ubase); i++)
        ((x_ubase *)stack_contex)[i] = 0xfadeface;

#ifdef  SEPARATE_COMPILE
    if (task->task_dync_sched_member.isolation_flag == 1) {
        stack_contex->ra      = (unsigned long)USERSPACE->us_taskquit;
    } else {
         stack_contex->ra      = (x_ubase)KTaskQuit;
    }
        
#else
    stack_contex->ra      = (x_ubase)KTaskQuit;
#endif

    stack_contex->a0      = (x_ubase)task->task_base_info.func_param;
    stack_contex->epc     = (x_ubase)task->task_base_info.func_entry;

#ifdef  TASK_ISOLATION
    stack_contex->tp     = (x_ubase)task;
    if(task->task_dync_sched_member.isolation_flag == 1)
        stack_contex->mstatus = 0x00006080;
    else
#endif
        stack_contex->mstatus = 0x00007880;

    return EOK;
}

#ifdef  TASK_ISOLATION
void RestoreMstatus(uintptr_t *sp)
{
    struct TaskDescriptor *tid;
    tid = (struct TaskDescriptor *)(sp[4]);
    if(tid->task_dync_sched_member.isolation_flag == 1 && tid->task_dync_sched_member.isolation_status == 0){
        CLEAR_CSR(mstatus, (MSTATUS_MPP));
#ifdef MOMERY_PROTECT_ENABLE
        mem_access.Load(tid->task_dync_sched_member.isolation);
#endif
    }else{
        SET_CSR(mstatus, MSTATUS_MPP);
    }
}
#endif
