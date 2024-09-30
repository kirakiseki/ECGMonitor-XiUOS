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
* @file:   isr.h
* @brief: function declaration and structure defintion of isr
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/3/10
*
*/

#ifndef __ISR_H__
#define __ISR_H__

#include <stdint.h>
#include <arch_interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DECLARE_HW_IRQ(_irq_num, _handler, _arg) \
    const uint32_t __irq_desc_idx_##_handler SECTION(".isrtbl.idx") = _irq_num +   ARCH_IRQ_NUM_OFFSET  ; \
    const struct IrqDesc __irq_desc_##_handler SECTION(".isrtbl") = { \
        .handler = _handler, \
        .param = _arg, \
    }

typedef void (*IsrHandlerType)(int vector, void *param);

struct IrqDesc
{
    IsrHandlerType handler;
    void *param;

#ifdef CONFIG_INTERRUPT_INFO
    char name[NAME_NUM_MAX];
    uint32_t counter;
#endif
};

struct IsrDone
{
    bool (*isInIsr)();
    int32_t  (*registerIrq)(uint32_t irq_num, IsrHandlerType handler, void *arg);
    int32_t  (*freeIrq)(uint32_t irq_num);
    int32_t  (*enableIrq)(uint32_t irq_num, uint32_t cpu_id);
    int32_t  (*disableIrq)(uint32_t irq_num, uint32_t cpu_id);
    void   (*handleIrq)(uint32_t irq_num);
    uint16_t (*getCounter)() ;
    void   (*incCounter)();
    void   (*decCounter)();
    uint8_t  (*getSwitchTrigerFlag)();
    void   (*setSwitchTrigerFlag)();
    void   (*clearSwitchTrigerFlag)();
};

struct InterruptServiceRoutines {

#ifdef ARCH_SMP
       volatile uint16_t isr_count[CPU_NUMBERS];
       volatile uint8_t  isr_switch_trigger_flag[CPU_NUMBERS];
#else
       volatile uint16_t isr_count ;
       volatile uint8_t  isr_switch_trigger_flag;
#endif
    struct IrqDesc irq_table[ARCH_MAX_IRQ_NUM];
    struct IsrDone *done;
};

extern struct InterruptServiceRoutines isrManager ;

uint32_t DisableLocalInterrupt();
void EnableLocalInterrupt(unsigned long level);

#define DISABLE_INTERRUPT DisableLocalInterrupt
#define ENABLE_INTERRUPT  EnableLocalInterrupt

void SysInitIsrManager();
void InitHwinterrupt(void);


#ifdef __cplusplus
}
#endif

#endif
