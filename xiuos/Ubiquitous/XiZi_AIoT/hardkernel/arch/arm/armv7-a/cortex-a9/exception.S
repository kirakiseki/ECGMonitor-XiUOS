#include <asm_defines.h>

.section .text.vectors, "ax"
.code 32

.globl ExceptionVectors
ExceptionVectors:
  ldr pc, _ResetException
  ldr pc, _UndefInstrException
  ldr pc, _SwiException
  ldr pc, _PrefetchAbortException
  ldr pc, _DataAbortAbortException
  ldr pc, _ResvException
  ldr pc, _IrqException
  ldr pc, _FiqException

.globl _reset
.globl UndefInstrExceptionHandle
.globl SwiExceptionHandle
.globl PrefetchAbortExceptionHandle
.globl DataAbortExceptionHandle
.globl ResvExceptionHandle
.globl ExceptionIsrEntry
.globl FiqExceptionHandle

_ResetException:
  .word _reset
_UndefInstrException:
  .word UndefInstrExceptionHandle
_SwiException:
  .word SwiExceptionHandle
_PrefetchAbortException:
  .word PrefetchAbortExceptionHandle
_DataAbortAbortException:
  .word DataAbortExceptionHandle
_ResvException:
  .word ResvExceptionHandle
_IrqException:
  .word ExceptionIsrEntry
_FiqException:
  .word FiqExceptionHandle

  .word 0 // extra word in RAM vectors


.macro push_svc_reg
    sub     sp, sp, #17 * 4         @/* Sizeof(struct rt_hw_exp_stack)  */
    stmia   sp, {r0 - r12}          @/* Calling r0-r12                  */
    mov     r0, sp
    mrs     r6, spsr                @/* Save CPSR                       */
    str     lr, [r0, #15*4]         @/* Push PC                         */
    str     r6, [r0, #16*4]         @/* Push CPSR                       */
    cps     #MODE_SVC
    str     sp, [r0, #13*4]         @/* Save calling SP                 */
    str     lr, [r0, #14*4]         @/* Save calling PC                 */
.endm

.align  5
.globl	UndefInstrExceptionHandle
UndefInstrExceptionHandle:
1:
  b       1b

.align  5
.globl	SwiExceptionHandle
SwiExceptionHandle:
  push_svc_reg
  bl      DoSvcCallProcess
  b       .

.align  5
.globl	PrefetchAbortExceptionHandle
PrefetchAbortExceptionHandle:
1:
  b       1b

.align  5
.globl	DataAbortExceptionHandle
DataAbortExceptionHandle:
1:
  b       1b

.align  5
.globl	ResvExceptionHandle
ResvExceptionHandle:
1:
  b       1b

.section .text.isr, "ax"
.align  5
.globl	ExceptionIsrEntry
ExceptionIsrEntry:

  stmfd   sp!, {r0-r12,lr}

  bl      DoIrqProcess

  @ ldr     r0, =rt_thread_switch_interrupt_flag
  @ ldr     r1, [r0]
  @ cmp     r1, #1
  @ beq     rt_hw_context_switch_interrupt_do

  ldmfd   sp!, {r0-r12,lr}
  subs    pc,  lr, #4

@ rt_hw_context_switch_interrupt_do:
@     mov     r1,  #0         @ clear flag
@     str     r1,  [r0]

@     mov     r1, sp          @ r1 point to {r0-r3} in stack
@     add     sp, sp, #4*4
@     ldmfd   sp!, {r4-r12,lr}@ reload saved registers
@     mrs     r0,  spsr       @ get cpsr of interrupt thread
@     sub     r2,  lr, #4     @ save old task's pc to r2

@     @ Switch to SVC mode with no interrupt. If the usr mode guest is
@     @ interrupted, this will just switch to the stack of kernel space.
@     @ save the registers in kernel space won't trigger data abort.
@     msr     cpsr_c, #I_Bit|F_Bit|Mode_SVC

@     stmfd   sp!, {r2}       @ push old task's pc
@     stmfd   sp!, {r4-r12,lr}@ push old task's lr,r12-r4
@     ldmfd   r1,  {r1-r4}    @ restore r0-r3 of the interrupt thread
@     stmfd   sp!, {r1-r4}    @ push old task's r0-r3
@     stmfd   sp!, {r0}       @ push old task's cpsr

@     ldr     r4,  =rt_interrupt_from_thread
@     ldr     r5,  [r4]
@     str     sp,  [r5]       @ store sp in preempted tasks's TCB

@     ldr     r6,  =rt_interrupt_to_thread
@     ldr     r6,  [r6]
@     ldr     sp,  [r6]       @ get new task's stack pointer

@     ldmfd   sp!, {r4}       @ pop new task's cpsr to spsr
@     msr     spsr_cxsf, r4

@     ldmfd   sp!, {r0-r12,lr,pc}^ @ pop new task's r0-r12,lr & pc, copy spsr to cpsr
 

.align  5
.globl	FiqExceptionHandle
FiqExceptionHandle:
1:
  b       1b
