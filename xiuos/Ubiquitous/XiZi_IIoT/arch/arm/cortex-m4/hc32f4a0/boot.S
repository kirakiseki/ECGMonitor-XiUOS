;/*****************************************************************************
; * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
; *
; * This software component is licensed by XHSC under BSD 3-Clause license
; * (the "License"); You may not use this file except in compliance with the
; * License. You may obtain a copy of the License at:
; *                    opensource.org/licenses/BSD-3-Clause
; *
; */
/*****************************************************************************/
/*  Startup for GCC                                                          */
/*  Version     V1.0                                                         */
/*  Date        2022-03-31                                                   */
/*  Target-mcu  HC32F4A0                                                     */
/*****************************************************************************/

/**
* @file boot.S
* @brief derived from XHSC library
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-08
*/

/*************************************************
File name: boot.S
Description: Reset and init function
Others: 
History: 
1. Date: 2022-09-08
Author: AIIT XUOS Lab
Modification: 
1. take startup_hc32f4a0.S for XiZi kernel
*************************************************/

/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

                .syntax     unified
                .arch       armv7e-m
                .cpu        cortex-m4
                .fpu        softvfp
                .thumb

/*
;<h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>
*/
                .equ        Stack_Size, 0x00004000

                .section    .stack
                .align      3
                .globl      __StackTop
                .globl      __StackLimit
__StackLimit:
                .space      Stack_Size
                .size       __StackLimit, . - __StackLimit
__StackTop:
                .size       __StackTop, . - __StackTop

/*
;<h> Reset handler start.
*/
                .section    .text.Reset_Handler
                .align      2
                .weak       Reset_Handler
                .type       Reset_Handler, %function
                .globl      Reset_Handler
Reset_Handler:
/* Single section scheme.
 *
 * The ranges of copy from/to are specified by following symbols
 *   __etext: LMA of start of the section to copy from. Usually end of text
 *   __data_start__: VMA of start of the section to copy to
 *   __data_end__: VMA of end of the section to copy to
 *
 * All addresses must be aligned to 4 bytes boundary.
 */
StackInit:
                ldr         r1, =__StackLimit
                ldr         r2, =__StackTop

                movs        r0, 0
StackInitLoop:
                cmp         r1, r2
                itt         lt
                strlt       r0, [r1], #4
                blt         StackInitLoop

ClrSramSR:
                ldr         r0, =0x40050810
                movw        r1, #0x1FF
                str         r1, [r0]

                /* Copy data from read only memory to RAM. */
CopyData:
                ldr         r1, =__etext
                ldr         r2, =__data_start__
                ldr         r3, =__data_end__
CopyLoop:
                cmp         r2, r3
                ittt        lt
                ldrlt       r0, [r1], #4
                strlt       r0, [r2], #4
                blt         CopyLoop

CopyData1:
                ldr         r1, =__etext_ramb
                ldr         r2, =__data_start_ramb__
                ldr         r3, =__data_end_ramb__
CopyLoop1:
                cmp         r2, r3
                ittt        lt
                ldrlt       r0, [r1], #4
                strlt       r0, [r2], #4
                blt         CopyLoop1

/* This part of work usually is done in C library startup code.
 * Otherwise, define this macro to enable it in this startup.
 *
 * There are two schemes too.
 * One can clear multiple BSS sections. Another can only clear one section.
 * The former is more size expensive than the latter.
 *
 * Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 * Otherwise define macro __STARTUP_CLEAR_BSS to choose the later.
 */
/* Single BSS section scheme.
 *
 * The BSS section is specified by following symbols
 *   __bss_start__: start of the BSS section.
 *   __bss_end__: end of the BSS section.
 *
 * Both addresses must be aligned to 4 bytes boundary.
 */
                /* Clear BSS section. */
ClearBss:
                ldr         r1, =__bss_start__
                ldr         r2, =__bss_end__

                movs        r0, 0
ClearLoop:
                cmp         r1, r2
                itt         lt
                strlt       r0, [r1], #4
                blt         ClearLoop

ClearBss1:
                ldr         r1, =__bss_start_ramb__
                ldr         r2, =__bss_end_ramb__

                movs        r0, 0
ClearLoop1:
                cmp         r1, r2
                itt         lt
                strlt       r0, [r1], #4
                blt         ClearLoop1

                /* Call the clock system initialization function. */
                bl          SystemInit
                /* Call the application's entry point. */
                bl          entry
                bx          lr
                .size       Reset_Handler, . - Reset_Handler
/*
;<h> Reset handler end.
*/

/*
;<h> Default handler start.
*/
                .section    .text.Default_Handler, "ax", %progbits
                .align      2
Default_Handler:
                b           .
                .size       Default_Handler, . - Default_Handler
/*
;<h> Default handler end.
*/
