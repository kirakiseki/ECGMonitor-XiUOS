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

/**
* @file:    isr.c
* @brief:   the general management of system isr
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/3/15
*
*/
#include <string.h>
#include "isr.h"

struct InterruptServiceRoutines isrManager = {0} ;

#ifdef ARCH_SMP
extern int GetCpuId(void);
#endif
/**
 * This functionwill get the isr nest level.
 *
 * @return isr nest level
 */
static uint16_t GetIsrCounter()
{
    uint16_t ret = 0;

#ifdef ARCH_SMP
    ret = isrManager.isr_count[GetCpuId()];
#else
    ret = isrManager.isr_count;
#endif
    return ret;
}

static void IncIsrCounter()
{
#ifdef ARCH_SMP
     isrManager.isr_count[GetCpuId()] ++ ;
#else
     isrManager.isr_count ++;
#endif
    return ;
}

static void DecIsrCounter()
{

#ifdef ARCH_SMP
     isrManager.isr_count[GetCpuId()] -- ;
#else
     isrManager.isr_count --;
#endif
    return ;
}

bool IsInIsr()
{
#ifdef ARCH_SMP
    return ( isrManager.isr_count[GetCpuId()] != 0 ? TRUE : FALSE ) ;
#else
    return ( isrManager.isr_count != 0 ? TRUE : FALSE ) ;
#endif

}
/**
 * This function will register a new irq.
 *
 * @param irq_num the number of the irq
 * @param handler the callback of the interrupt
 * @param arg param of thge callback
 *
 * @return 0 on success; -1 on failure
 */
static int32_t RegisterHwIrq(uint32_t irq_num, IsrHandlerType handler, void *arg)
{
    if (irq_num >=   ARCH_MAX_IRQ_NUM )
        return -1;

    struct IrqDesc *desc = &isrManager.irq_table[irq_num];

    desc->handler = handler;
    desc->param = arg;

    return 0;
}
/**
 * This function will free a irq.
 *
 * @param irq_num the number of the irq
 *
 * @return 0 on success; -1 on failure
 */
static int32_t FreeHwIrq(uint32_t irq_num)
{
    if (irq_num >=   ARCH_MAX_IRQ_NUM  )
        return -1;

    memset(&isrManager.irq_table[irq_num], 0, sizeof(struct IrqDesc));

    return 0;
}

/**
 * This function will enable a irq.
 *
 * @param irq_num the number of the irq
 *
 * @return 0 on success; -1 on failure
 */
static int32_t EnableHwIrq(uint32_t irq_num, uint32_t cpu_id)
{
    if (irq_num >=   ARCH_MAX_IRQ_NUM  )
        return -1;

    return ArchEnableHwIrq(irq_num, cpu_id);
}
/**
 * This function will disable a irq.
 *
 * @param irq_num the number of the irq
 *
 * @return 0 on success; -1 on failure
 */

static int32_t DisableHwIrq(uint32_t irq_num, uint32_t cpu_id)
{
    if (irq_num >=   ARCH_MAX_IRQ_NUM  )
        return -1;

    return ArchDisableHwIrq(irq_num, cpu_id);
}

/* called from arch-specific ISR wrapper */
static void IsrCommon(uint32_t irq_num)
{
    struct IrqDesc *desc = &isrManager.irq_table[irq_num];

    if (desc->handler == NULL) {
        // SYS_KDEBUG_LOG(KDBG_IRQ, ("Spurious interrupt: IRQ No. %d\n", irq_num));
        while (1) {}
    }
    desc->handler(irq_num, desc->param);

}

static void SetIsrSwitchTrigerFlag()
{

#ifdef ARCH_SMP
    isrManager.isr_switch_trigger_flag[GetCpuId()] = 1;
#else
    isrManager.isr_switch_trigger_flag = 1;
#endif
}

static void ClearIsrSwitchTrigerFlag()
{

#ifdef ARCH_SMP
    isrManager.isr_switch_trigger_flag[GetCpuId()] = 0;
#else
    isrManager.isr_switch_trigger_flag = 0;
#endif
}

static uint8_t GetIsrSwitchTrigerFlag()
{
   
#ifdef ARCH_SMP
   return  isrManager.isr_switch_trigger_flag[GetCpuId()];
#else
   return isrManager.isr_switch_trigger_flag ;
#endif
}

struct IsrDone isrDone = {
    IsInIsr,
    RegisterHwIrq ,
    FreeHwIrq,
    EnableHwIrq,
    DisableHwIrq,
    IsrCommon,
    GetIsrCounter,
    IncIsrCounter,
    DecIsrCounter,
    GetIsrSwitchTrigerFlag,
    SetIsrSwitchTrigerFlag,
    ClearIsrSwitchTrigerFlag
};

void SysInitIsrManager()
{
    extern int __isrtbl_idx_start;
    extern int __isrtbl_start;
    extern int __isrtbl_end;
    memset(&isrManager,0,sizeof(struct InterruptServiceRoutines));
    isrManager.done = &isrDone;

    uint32_t *index = (uint32_t *)&__isrtbl_idx_start;
    struct IrqDesc *desc = (struct IrqDesc *)&__isrtbl_start;

    while (desc != (struct IrqDesc *)&__isrtbl_end)
        isrManager.irq_table[*index++] = *desc++;
}
