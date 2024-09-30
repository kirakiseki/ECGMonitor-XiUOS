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
* @file interrupt_vector.S
* @brief derived from XHSC library
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-08
*/

/*************************************************
File name: interrupt_vector.S
Description: Interrupt Vectors
Others: 
History: 
1. Date: 2022-09-08
Author: AIIT XUOS Lab
Modification: 
1. take startup_hc32f4a0.s for XiZi kernel
*************************************************/

/*
;<h> Interrupt vector table start.
*/
                .section    .isr_vector, "a", %progbits
                .align      2
                .type       InterruptVectors, %object
                .globl      InterruptVectors
                .globl      InterruptVectors_End
                .globl      InterruptVectors_Size
InterruptVectors:
                .long       __StackTop                         /*     Top of Stack */
                .long       Reset_Handler                      /*     Reset Handler */
                .long       NMI_Handler                        /* -14 NMI Handler */
                .long       HardFaultHandler                   /* -13 Hard Fault Handler */
                .long       MemFaultHandler                    /* -12 MPU Fault Handler */
                .long       BusFault_Handler                   /* -11 Bus Fault Handler */
                .long       UsageFault_Handler                 /* -10 Usage Fault Handler */
                .long       0                                  /*     Reserved */
                .long       0                                  /*     Reserved */
                .long       0                                  /*     Reserved */
                .long       0                                  /*     Reserved */
                .long       IsrEntry                           /*  -5 SVCall Handler */
                .long       IsrEntry                           /*  -4 Debug Monitor Handler */
                .long       0                                  /*     Reserved */
                .long       PendSV_Handler                     /*  -2 PendSV Handler */
                .long       SysTick_Handler                    /*  -1 SysTick Handler */

                /* Interrupts */
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IsrEntry
                .long       IRQ046_Handler
                .long       IRQ047_Handler
                .long       IRQ048_Handler
                .long       IRQ049_Handler
                .long       IRQ050_Handler
                .long       IRQ051_Handler
                .long       IRQ052_Handler
                .long       IRQ053_Handler
                .long       IRQ054_Handler
                .long       IRQ055_Handler
                .long       IRQ056_Handler
                .long       IRQ057_Handler
                .long       IRQ058_Handler
                .long       IRQ059_Handler
                .long       IRQ060_Handler
                .long       IRQ061_Handler
                .long       IRQ062_Handler
                .long       IRQ063_Handler
                .long       IRQ064_Handler
                .long       IRQ065_Handler
                .long       IRQ066_Handler
                .long       IRQ067_Handler
                .long       IRQ068_Handler
                .long       IRQ069_Handler
                .long       IRQ070_Handler
                .long       IRQ071_Handler
                .long       IRQ072_Handler
                .long       IRQ073_Handler
                .long       IRQ074_Handler
                .long       IRQ075_Handler
                .long       IRQ076_Handler
                .long       IRQ077_Handler
                .long       IRQ078_Handler
                .long       IRQ079_Handler
                .long       IRQ080_Handler
                .long       IRQ081_Handler
                .long       IRQ082_Handler
                .long       IRQ083_Handler
                .long       IRQ084_Handler
                .long       IRQ085_Handler
                .long       IRQ086_Handler
                .long       IRQ087_Handler
                .long       IRQ088_Handler
                .long       IRQ089_Handler
                .long       IRQ090_Handler
                .long       IRQ091_Handler
                .long       IRQ092_Handler
                .long       IRQ093_Handler
                .long       IRQ094_Handler
                .long       IRQ095_Handler
                .long       IRQ096_Handler
                .long       IRQ097_Handler
                .long       IRQ098_Handler
                .long       IRQ099_Handler
                .long       IRQ100_Handler
                .long       IRQ101_Handler
                .long       IRQ102_Handler
                .long       IRQ103_Handler
                .long       IRQ104_Handler
                .long       IRQ105_Handler
                .long       IRQ106_Handler
                .long       IRQ107_Handler
                .long       IRQ108_Handler
                .long       IRQ109_Handler
                .long       IRQ110_Handler
                .long       IRQ111_Handler
                .long       IRQ112_Handler
                .long       IRQ113_Handler
                .long       IRQ114_Handler
                .long       IRQ115_Handler
                .long       IRQ116_Handler
                .long       IRQ117_Handler
                .long       IRQ118_Handler
                .long       IRQ119_Handler
                .long       IRQ120_Handler
                .long       IRQ121_Handler
                .long       IRQ122_Handler
                .long       IRQ123_Handler
                .long       IRQ124_Handler
                .long       IRQ125_Handler
                .long       IRQ126_Handler
                .long       IRQ127_Handler
                .long       IRQ128_Handler
                .long       IRQ129_Handler
                .long       IRQ130_Handler
                .long       IRQ131_Handler
                .long       IRQ132_Handler
                .long       IRQ133_Handler
                .long       IRQ134_Handler
                .long       IRQ135_Handler
                .long       IRQ136_Handler
                .long       IRQ137_Handler
                .long       IRQ138_Handler
                .long       IRQ139_Handler
                .long       IRQ140_Handler
                .long       IRQ141_Handler
                .long       IRQ142_Handler
                .long       IRQ143_Handler
InterruptVectors_End:
                .equ        InterruptVectors_Size, InterruptVectors_End - InterruptVectors
                .size       InterruptVectors, . - InterruptVectors
/*
;<h> Interrupt vector table end.
*/
