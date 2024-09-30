// extern void _svcall(uintptr_t* contex);
#include <stdint.h>
#include <stddef.h>
#include <isr.h>

uint32_t DisableLocalInterrupt(void)
{
    uint32_t intSave;
    __asm__ __volatile__(
        "mrs    %0, cpsr      \n"
        "cpsid  if              "
        : "=r"(intSave)
        :
        : "memory");
    return intSave;
}

void EnableLocalInterrupt(unsigned long level)
{
    uint32_t intSave;
    __asm__ __volatile__(
        "mrs    %0, cpsr      \n"
        "cpsie  if              "
        : "=r"(intSave)
        :
        : "memory");
    return;
}

int32_t ArchEnableHwIrq(uint32_t irq_num, uint32_t cpu_id)
{
    // gic_set_irq_priority(irq_num, priority);
    gic_set_irq_security(irq_num, false);    // set IRQ as non-secure
    gic_set_cpu_target(irq_num, cpu_id, true);
    gic_enable_irq(irq_num, true);
    return 0;
}

int32_t ArchDisableHwIrq(uint32_t irq_num, uint32_t cpu_id)
{
    gic_enable_irq(irq_num, false);
    gic_set_cpu_target(irq_num, cpu_id, false);
    return 0;
}

extern  void KTaskOsAssignAfterIrq(void *context);

void IsrEntry(uint32_t irq_num)
{
    isrManager.done->incCounter();
    isrManager.done->handleIrq(irq_num);
    // KTaskOsAssignAfterIrq(NULL);
    isrManager.done->decCounter();
 
}

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */
void PrintStackFrame(struct ExceptionStackRegister *regs)
{
    // KPrintf("Execption:\n");
    // KPrintf("r0:     0x%08x\n", regs->r0);
    // KPrintf("r1:     0x%08x\n", regs->r1);
    // KPrintf("r2:     0x%08x\n", regs->r2);
    // KPrintf("r3:     0x%08x\n", regs->r3);
    // KPrintf("r4:     0x%08x\n", regs->r4);
    // KPrintf("r5:     0x%08x\n", regs->r5);
    // KPrintf("r6:     0x%08x\n", regs->r6);
    // KPrintf("r7:     0x%08x\n", regs->r7);
    // KPrintf("r8:     0x%08x\n", regs->r8);
    // KPrintf("r9:     0x%08x\n", regs->r9);
    // KPrintf("r10:    0x%08x\n", regs->r10);
    // KPrintf("r11:    0x%08x\n", regs->r11);
    // KPrintf("r12:    0x%08x\n", regs->r12);
    // KPrintf("r13_sp: 0x%08x\n", regs->r13_sp);
    // KPrintf("r14_lr: 0x%08x\n", regs->r14_lr);
    // KPrintf("r15_pc: 0x%08x\n", regs->r15_pc);
    // KPrintf("cpsr:   0x%08x\n", regs->cpsr);
}


void DoSvcCallProcess(struct ExceptionStackRegister *regs)
{

}

void DoIrqProcess(void)
{
    uint32_t iar = gic_read_irq_ack();
    uint32_t irq_num = iar & 0x3ff;

    if(irq_num >= ARCH_MAX_IRQ_NUM)
    {
        gic_write_end_of_irq(irq_num);
        return;
    }

    IsrEntry(irq_num);

    gic_write_end_of_irq(irq_num);
}
// uintptr_t *Svcall(unsigned int ipsr,  uintptr_t* contex )
// {
// #ifdef TASK_ISOLATION
//     _svcall(contex);
// #endif
//     return contex;
// }