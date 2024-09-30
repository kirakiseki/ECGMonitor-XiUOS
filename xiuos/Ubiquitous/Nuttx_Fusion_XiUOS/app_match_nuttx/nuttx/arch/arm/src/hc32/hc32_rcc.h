/****************************************************************************
 * arch/arm/src/hc32/hc32_rcc.h
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

#ifndef __ARCH_ARM_SRC_HC32_HC32_RCC_H
#define __ARCH_ARM_SRC_HC32_HC32_RCC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "arm_internal.h"
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* This symbol references the Cortex-M3/4 vector table (as positioned by the
 * linker script, ld.script or ld.script.dfu.  The standard location for the
 * vector table is at the beginning of FLASH at address 0x0800:0000.  If we
 * are using the STMicro DFU bootloader, then the vector table will be offset
 * to a different location in FLASH and we will need to set the NVIC vector
 * location to this alternative location.
 */

#if defined(__ICCARM__)
/* _vectors replaced on __vector_table for IAR C-SPY Simulator */

extern uint32_t __vector_table[];
#else
extern uint32_t _vectors[];
#endif


/****************************************************************************
 * Inline Functions
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: hc32_clockconfig
 *
 * Description:
 *   Called to establish the clock settings based on the values in board.h.
 *   This function (by default) will reset most everything, enable the PLL,
 *   and enable peripheral clocking for all periperipherals enabled in the
 *   NuttX configuration file.
 *
 *   If CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG is defined, then clocking
 *   will be enabled by an externally provided, board-specific function
 *   called hc32_board_clockconfig().
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void hc32_clockconfig(void);

/****************************************************************************
 * Name: hc32_board_clockconfig
 *
 * Description:
 *   Any HC32 board may replace the "standard" board clock configuration
 *   logic with its own, custom clock configuration logic.
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG
void hc32_board_clockconfig(void);
#endif

/****************************************************************************
 * Name: hc32_clockenable
 *
 * Description:
 *   Re-enable the clock and restore the clock settings based on settings in
 *   board.h.
 *   This function is only available to support low-power modes of operation:
 *   When re-awakening from deep-sleep modes, it is necessary to re-enable/
 *   re-start the PLL
 *
 *   This functional performs a subset of the operations performed by
 *   hc32_clockconfig():  It does not reset any devices, and it does not
 *   reset the currently enabled peripheral clocks.
 *
 *   If CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG is defined, then clocking
 *   will be enabled by an externally provided, board-specific function
 *   called hc32_board_clockconfig().
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#ifdef CONFIG_PM
void hc32_clockenable(void);
#endif

/****************************************************************************
 * Name: hc32_rcc_enablelse
 *
 * Description:
 *   Enable the External Low-Speed (LSE) Oscillator.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void hc32_rcc_enablelse(void);

/****************************************************************************
 * Name: hc32_rcc_enablelsi
 *
 * Description:
 *   Enable the Internal Low-Speed (LSI) RC Oscillator.
 *
 ****************************************************************************/

void hc32_rcc_enablelsi(void);

/****************************************************************************
 * Name: hc32_rcc_disablelsi
 *
 * Description:
 *   Disable the Internal Low-Speed (LSI) RC Oscillator.
 *
 ****************************************************************************/

void hc32_rcc_disablelsi(void);

void hc32_clk_init(void);

void hc32_clk_config(void);

void hc32_sysclk_init(void);             /*!< Initialize the system */

void hc32_sysclk_update(void);  /*!< Update SystemCoreClock variable */

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_HC32_HC32_RCC_H */
