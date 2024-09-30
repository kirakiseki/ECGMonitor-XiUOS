/****************************************************************************
 * arch/arm/src/hc32/hc32_gpio.h
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

#ifndef __ARCH_ARM_SRC_HC32_HC32_GPIO_H
#define __ARCH_ARM_SRC_HC32_HC32_GPIO_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#  include <stdbool.h>
#endif

#include <nuttx/irq.h>
#include "chip.h"


/****************************************************************************
 * Pre-Processor Declarations
 ****************************************************************************/
typedef uint32_t gpio_pinset_t;

#define GPIO_OUTPUT_SET               (1 << 25)                   /* Bit 8: If output, initial value of output */
#define GPIO_OUTPUT_CLEAR             (0)

#define GPIO_MODE_SHIFT               (18)                       /* Bits 18-19: GPIO port mode */
#define GPIO_MODE_MASK                (3 << GPIO_MODE_SHIFT)
#  define GPIO_INPUT                  (0 << GPIO_MODE_SHIFT)     /* Input mode */
#  define GPIO_OUTPUT                 (1 << GPIO_MODE_SHIFT)     /* General purpose output mode */
#  define GPIO_ALT                    (2 << GPIO_MODE_SHIFT)     /* Alternate function mode */
#  define GPIO_ANALOG                 (3 << GPIO_MODE_SHIFT)     /* Analog mode */

/****************************************************************************
 * Public Data
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
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: hc32_configgpio
 *
 * Description:
 *   Configure a GPIO pin based on bit-encoded description of the pin.
 *   Once it is configured as Alternative (GPIO_ALT|GPIO_CNF_AFPP|...)
 *   function, it must be unconfigured with hc32_unconfiggpio() with
 *   the same cfgset first before it can be set to non-alternative function.
 *
 * Returned Value:
 *   OK on success
 *   ERROR on invalid port, or when pin is locked as ALT function.
 *
 ****************************************************************************/

int hc32_configgpio(uint32_t cfgset);

/****************************************************************************
 * Name: hc32_unconfiggpio
 *
 * Description:
 *   Unconfigure a GPIO pin based on bit-encoded description of the pin, set
 *   it into default HiZ state (and possibly mark it's unused) and unlock it
 *   whether it was previously selected as alternative function
 *   (GPIO_ALT|GPIO_CNF_AFPP|...).
 *
 *   This is a safety function and prevents hardware from shocks, as
 *   unexpected write to the Timer Channel Output GPIO to fixed '1' or '0'
 *   while it should operate in PWM mode could produce excessive on-board
 *   currents and trigger over-current/alarm function.
 *
 * Returned Value:
 *  OK on success
 *  ERROR on invalid port
 *
 ****************************************************************************/

int hc32_unconfiggpio(uint32_t cfgset);

/****************************************************************************
 * Name: hc32_gpiowrite
 *
 * Description:
 *   Write one or zero to the selected GPIO pin
 *
 ****************************************************************************/

void hc32_gpiowrite(uint32_t pinset, bool value);

/****************************************************************************
 * Name: hc32_gpioread
 *
 * Description:
 *   Read one or zero from the selected GPIO pin
 *
 ****************************************************************************/

bool hc32_gpioread(uint32_t pinset);

/****************************************************************************
 * Name: hc32_iocompensation
 *
 * Description:
 *   Enable I/O compensation.
 *
 *   By default the I/O compensation cell is not used. However when the I/O
 *   output buffer speed is configured in 50 MHz or 100 MHz mode, it is
 *   recommended to use the compensation cell for slew rate control on I/O
 *   tf(IO)out)/tr(IO)out commutation to reduce the I/O noise on power
 *   supply.
 *
 *   The I/O compensation cell can be used only when the supply voltage
 *   ranges from 2.4 to 3.6 V.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#ifdef CONFIG_HC32_HAVE_IOCOMPENSATION
void hc32_iocompensation(void);
#endif

/****************************************************************************
 * Function:  hc32_dumpgpio
 *
 * Description:
 *   Dump all GPIO registers associated with the provided base address
 *
 ****************************************************************************/

#ifdef CONFIG_DEBUG_FEATURES
int hc32_dumpgpio(uint32_t pinset, const char *msg);
#else
#  define hc32_dumpgpio(p,m)
#endif

/****************************************************************************
 * Function:  hc32_gpioinit
 *
 * Description:
 *   Based on configuration within the .config file, it does:
 *    - Remaps positions of alternative functions.
 *
 *   Typically called from hc32_start().
 *
 ****************************************************************************/

void hc32_gpioinit(void);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_HC32_HC32_GPIO_H */
