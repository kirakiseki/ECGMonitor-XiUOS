/****************************************************************************
 * arch/arm/include/HC32F4A0/irq.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/* This file should never be included directly but, rather,
 * only indirectly through nuttx/irq.h
 */

#ifndef __ARCH_ARM_INCLUDE_HC32F4A0_IRQ_H
#define __ARCH_ARM_INCLUDE_HC32F4A0_IRQ_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__
#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

    /* Processor Exceptions (vectors 0-15) */

#define HC32_IRQ_RESERVED       (0) /* Reserved vector (only used with CONFIG_DEBUG_FEATURES) */
                                         /* Vector  0: Reset stack pointer value */
                                         /* Vector  1: Reset (not handler as an IRQ) */
#define HC32_IRQ_NMI            (2) /* Vector  2: Non-Maskable Interrupt (NMI) */
#define HC32_IRQ_HARDFAULT      (3) /* Vector  3: Hard fault */
#define HC32_IRQ_MEMFAULT       (4) /* Vector  4: Memory management (MPU) */
#define HC32_IRQ_BUSFAULT       (5) /* Vector  5: Bus fault */
#define HC32_IRQ_USAGEFAULT     (6) /* Vector  6: Usage fault */
#define HC32_IRQ_SVCALL        (11) /* Vector 11: SVC call */
#define HC32_IRQ_DBGMONITOR    (12) /* Vector 12: Debug Monitor */
                                         /* Vector 13: Reserved */
#define HC32_IRQ_PENDSV        (14) /* Vector 14: Pendable system service request */
#define HC32_IRQ_SYSTICK       (15) /* Vector 15: System tick */

    /* External interrupts (vectors >= 16).
     * These definitions are chip-specific
     */

#define HC32_IRQ_FIRST         (16) /* Vector number of the first external interrupt */


#define NR_IRQS (144 + 15)

#define NVIC_SYSH_PRIORITY_MIN     0U// 0xf0
#define NVIC_SYSH_PRIORITY_DEFAULT 0U //0x80
#define NVIC_SYSH_DISABLE_PRIORITY    NVIC_SYSH_PRIORITY_DEFAULT

#define ARMV7M_PERIPHERAL_INTERRUPTS  NR_IRQS


#undef EXTERN
#ifdef __cplusplus
}
#endif
#endif

#endif /* __ARCH_ARM_INCLUDE_HC32F4A0_IRQ_H */
