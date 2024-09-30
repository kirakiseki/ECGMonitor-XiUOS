/****************************************************************************
 * arch/arm/src/hc32/hc32_gpio.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>

#include "arm_internal.h"
#include "chip.h"
#include <arch/board/board.h>
#include "hc32_gpio.h"
#include "hc32f4a0_gpio.h"
#include "hc32f4a0_utility.h"

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void hc32_gpio_flick(void)
{
    uint32_t pinset = (GPIO_PINSET(LED_GREEN_PORT, LED_GREEN_PIN) | GPIO_OUTPUT_SET);
    hc32_configgpio(pinset);

    for(int i = 0; i < 3; i++)
    {
        hc32_gpiowrite(pinset, true);
        DDL_DelayMS(200UL);
        hc32_gpiowrite(pinset, false);
        DDL_DelayMS(200UL);
    }
}

/****************************************************************************
 * Function:  hc32_gpioremap
 *
 * Description:
 *
 *   Based on configuration within the .config file, this function will
 *   remaps positions of alternative functions.
 *
 ****************************************************************************/

static inline void hc32_gpioremap(void)
{
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Function:  hc32_gpioinit
 *
 * Description:
 *   Based on configuration within the .config file, it does:
 *    - Remaps positions of alternative functions.
 *
 *   Typically called from hc32_start().
 *
 * Assumptions:
 *   This function is called early in the initialization sequence so that
 *   no mutual exclusion is necessary.
 *
 ****************************************************************************/

void hc32_gpioinit(void)
{
    /* Remap according to the configuration within .config file */
    hc32_gpioremap();
    hc32_gpio_flick();
}

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
 *   A negated errno value on invalid port, or when pin is locked as ALT
 *   function.
 *
 * To-Do: Auto Power Enable
 ****************************************************************************/

/****************************************************************************
 * Name: hc32_configgpio (for the HC32F4A0)
 ****************************************************************************/

int hc32_configgpio(uint32_t cfgset)
{
    unsigned int port;
    unsigned int pin;

    irqstate_t flags;
    stc_gpio_init_t stcGpioInit;

    /* Verify that this hardware supports the select GPIO port */
    port = (cfgset >> GPIO_PORT_SHIFT) & GPIO_PORT_MASK;
    if (port > GPIO_PORT_MASK)
    {
        return -EINVAL;
    }

    /* Get the pin number and select the port configuration register for that pin */
    pin = (cfgset >> GPIO_PIN_SHIFT) & GPIO_PIN_MASK;

    /* Interrupts must be disabled from here on out so that we have mutually
    * exclusive access to all of the GPIO configuration registers.
    */

    flags = enter_critical_section();

    /* If it is an output... set the pin to the correct initial state.
    * If it is pull-down or pull up, then we need to set the ODR
    * appropriately for that function.
    */
    GPIO_Unlock();
    /* LED initialize */
    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(port, pin, &stcGpioInit);
    GPIO_Lock();

    /* Register write protected for some required peripherals. */
    if ((cfgset & GPIO_OUTPUT_SET) != 0)
    {
        /* "Turn off" LED before set to output */
        GPIO_ResetPins(port, pin);
        GPIO_OE(port, pin, Enable);
    }
    else
    {
        GPIO_OE(port, pin, Disable);
    }

    leave_critical_section(flags);
    return OK;
}

/****************************************************************************
 * Name: hc32_unconfiggpio
 *
 * Description:
 *   Unconfigure a GPIO pin based on bit-encoded description of the pin, set
 *   it into default HiZ state (and possibly mark it's unused) and unlock it
 *   whether it was previously selected as an alternative function
 *   (GPIO_ALT | GPIO_CNF_AFPP | ...).
 *
 *   This is a safety function and prevents hardware from shocks, as
 *   unexpected write to the Timer Channel Output GPIO to fixed '1' or '0'
 *   while it should operate in PWM mode could produce excessive on-board
 *   currents and trigger over-current/alarm function.
 *
 * Returned Value:
 *  OK on success
 *  A negated errno value on invalid port
 *
 * To-Do: Auto Power Disable
 ****************************************************************************/

int hc32_unconfiggpio(uint32_t cfgset)
{
    /* Reuse port and pin number and set it to default HiZ INPUT */
    cfgset &= GPIO_PORT_MASK | GPIO_PIN_MASK;
    cfgset |= GPIO_INPUT;
    return hc32_configgpio(cfgset);
}

/****************************************************************************
 * Name: hc32_gpiowrite
 *
 * Description:
 *   Write one or zero to the selected GPIO pin
 *
 ****************************************************************************/

void hc32_gpiowrite(uint32_t pinset, bool value)
{
    unsigned int port;
    unsigned int pin;

    port = (pinset >> GPIO_PORT_SHIFT) & GPIO_PORT_MASK;
    if (port < GPIO_PORT_MASK)
    {
        /* Get the pin number  */
        pin = (pinset >> GPIO_PIN_SHIFT) & GPIO_PIN_MASK;

        /* Set or clear the output on the pin */
        if (value)
        {
            GPIO_SetPins(port, pin);
        }
        else
        {
            GPIO_ResetPins(port, pin);
        }
    }
}

/****************************************************************************
 * Name: hc32_gpioread
 *
 * Description:
 *   Read one or zero from the selected GPIO pin
 *
 ****************************************************************************/

bool hc32_gpioread(uint32_t pinset)
{
    unsigned int port;
    unsigned int pin;

    port = (pinset >> GPIO_PORT_SHIFT) & GPIO_PORT_MASK;
    if (port < GPIO_PORT_MASK)
    {
        /* Get the pin number and return the input state of that pin */
        pin = (pinset >> GPIO_PIN_SHIFT) & GPIO_PIN_MASK;
        return (GPIO_ReadInputPins(port, pin) == Pin_Set);
    }

    return 0;
}

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
void hc32_iocompensation(void)
{
#ifdef HC32_SYSCFG_CMPCR
  /* Enable I/O Compensation.  Writing '1' to the CMPCR power-down bit
   * enables the I/O compensation cell.
   */

#endif
}
#endif
