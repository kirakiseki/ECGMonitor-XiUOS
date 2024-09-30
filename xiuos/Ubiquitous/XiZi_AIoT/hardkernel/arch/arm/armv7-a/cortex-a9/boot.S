#include <asm_defines.h>

.global ExceptionVectors

.section ".startup","ax"
.globl _reset
.extern init
_reset:

    /* set the cpu to SVC32 mode and disable interrupt */
    mrs     r0, cpsr
    bic     r0, r0, #0x1f
    orr     r0, r0, #0x13
    msr     cpsr_c, r0

    /* disable i/d cache mmu*/
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, #(1 << 12)          /* i cache */
    bic     r0, #(1 << 2)           /* d cache */
    bic     r0, #(1 << 0)           /* mmu */
    mcr     p15, 0, r0, c1, c0, 0
    
    ldr     r0, =stack_top

    @ get cpu id, and subtract the offset from the stacks base address
    mrc     p15,0,r2,c0,c0,5  @ read multiprocessor affinity register
    and     r2, r2, #3        @ mask off, leaving CPU ID field
    mov     r5, r2		      @ save cpu id for later

    @  Set the startup stack for svc
    mov     sp, r0

    @  Enter Undefined Instruction Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_UND|I_BIT|F_BIT
    mov     sp, r0
    sub     r0, r0, #EXCEPTION_STACK_SIZE

    @  Enter Abort Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_ABT|I_BIT|F_BIT
    mov     sp, r0
    sub     r0, r0, #EXCEPTION_STACK_SIZE

    @  Enter FIQ Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_FIQ|I_BIT|F_BIT
    mov     sp, r0
    sub     r0, r0, #EXCEPTION_STACK_SIZE

    @  Enter IRQ Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_IRQ|I_BIT|F_BIT
    mov     sp, r0
    sub     r0, r0, #EXCEPTION_STACK_SIZE

    /* come back to SVC mode */
    msr     cpsr_c, #MODE_SVC|I_BIT|F_BIT

    /*
     * copy the vector table into the RAM vectors
     * this assumes that the RAM vectors size is divisible by 3 words (12 bytes)
     */
    ldr     r1,=__ram_vectors_start
    ldr     r2,=__ram_vectors_end
    ldr     r3,=ExceptionVectors
1:  cmp     r1,r2
    ldmlt   r3!,{r4,r5,r6}
    stmlt   r1!,{r4,r5,r6}
    blt     1b

    /* clear .bss */
    mov     r0, #0                  /* get a zero */
    ldr     r1,=__bss_start           /* bss start  */
    ldr     r2,=__bss_end             /* bss end    */

bss_loop:
    cmp     r1,r2                   /* check if data to clear           */
    strlo   r0,[r1],#4              /* clear 4 bytes                    */
    blo     bss_loop                /* loop until done                  */

    /* call C++ constructors of global objects                          */
    ldr     r0, =__ctors_start__
    ldr     r1, =__ctors_end__
bss_end:

ctor_loop:
    cmp     r0, r1
    beq     ctor_end
    ldr     r2, [r0], #4
    stmfd   sp!, {r0-r1}
    mov     lr, pc
    bx      r2
    ldmfd   sp!, {r0-r1}
    b       ctor_loop
ctor_end:

    bl     init

_loop_here:
    b      _loop_here