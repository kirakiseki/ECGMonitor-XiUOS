/****************************************************************************
 * boards/arm/imxrt/xidatong-arm32/src/imxrt_gt9xx.c
 *
 *   Copyright 2019 ElFaro LAB S.L. All rights reserved.
 *   Author: Fabio Balzano <fabio@elfarolab.com>
 *
 *   Based on boards/arm/lpc54xx/lpcxpresso-lpc54628/src/lpc54_ft5x06.c
 *   Copyright (C) 2017 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file imxrt_gt9xx.c
 * @brief gt9xx touch driver refer to imxrt_ft5x06.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.5.31
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <syslog.h>
#include <assert.h>
#include <errno.h>

#include <nuttx/arch.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/input/gt9xx.h>

#include "imxrt_config.h"
#include "imxrt_gpio.h"
#include "imxrt_lpi2c.h"

#include "arch/chip/irq.h"
#include "imxrt_iomuxc.h"

#define gt_print printf

#define GT9XX_I2C_ADDRESS  0x5D

#define GPIO_GT9XX_INTR     IMXRT_IRQ_GPIO2_30

#define IOMUX_GT9XX_RST     (IOMUX_PULL_NONE | IOMUX_CMOS_OUTPUT | \
                             IOMUX_DRIVE_40OHM | IOMUX_SPEED_MEDIUM | \
                             IOMUX_SLEW_SLOW)

#define GPIO_GT9XX_CTRSTN   (GPIO_OUTPUT | GPIO_OUTPUT_ZERO | \
                             GPIO_PORT2 | GPIO_PIN30 | IOMUX_GT9XX_RST)

#define GPIO_GT9XX_CTINTN   (GPIO_INTERRUPT | GPIO_INT_RISINGEDGE | \
                             IOMUX_SW_DEFAULT | GPIO_PORT2 | GPIO_PIN30 )

#ifdef CONFIG_INPUT_GT9XX

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GT9XX_FREQUENCY 400000

/****************************************************************************
 * Private Function Ptototypes
 ****************************************************************************/

#ifndef CONFIG_GT9XX_POLLMODE
static int  imxrt_gt9xx_attach(FAR const struct gt9xx_config_s *config,
              xcpt_t isr, FAR void *arg);
static void imxrt_gt9xx_enable(FAR const struct gt9xx_config_s *config,
              bool enable);
static void imxrt_gt9xx_clear(FAR const struct gt9xx_config_s *config);
#endif

static void imxrt_gt9xx_wakeup(FAR const struct gt9xx_config_s *config);
static void imxrt_gt9xx_nreset(FAR const struct gt9xx_config_s *config,
              bool state);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct gt9xx_config_s g_gt9xx_config =
{
  .address   = GT9XX_I2C_ADDRESS,
  .frequency = GT9XX_FREQUENCY,
#ifndef CONFIG_GT9XX_POLLMODE
  .attach    = imxrt_gt9xx_attach,
  .enable    = imxrt_gt9xx_enable,
  .clear     = imxrt_gt9xx_clear,
#endif
  .wakeup    = imxrt_gt9xx_wakeup,
  .nreset    = imxrt_gt9xx_nreset
};

#ifndef CONFIG_GT9XX_POLLMODE
static uint8_t g_gt9xx_irq;
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: imxrt_gt9xx_attach
 *
 * Description:
 *   Attach an GT9XX interrupt handler to a GPIO interrupt
 *
 ****************************************************************************/

#ifndef CONFIG_GT9XX_POLLMODE
static int imxrt_gt9xx_attach(FAR const struct gt9xx_config_s *config,
                               xcpt_t isr, FAR void *arg)
{
  return irq_attach(g_gt9xx_irq, isr, arg);
}
#endif

/****************************************************************************
 * Name: imxrt_gt9xx_enable
 *
 * Description:
 *   Enable or disable a GPIO interrupt
 *
 ****************************************************************************/

#ifndef CONFIG_GT9XX_POLLMODE
static void imxrt_gt9xx_enable(FAR const struct gt9xx_config_s *config,
                                bool enable)
{
  if (enable)
    {
      up_enable_irq(g_gt9xx_irq);
    }
  else
    {
      up_disable_irq(g_gt9xx_irq);
    }
}
#endif

/****************************************************************************
 * Name: imxrt_gt9xx_clear
 *
 * Description:
 *   Acknowledge/clear any pending GPIO interrupt
 *
 ****************************************************************************/

#ifndef CONFIG_GT9XX_POLLMODE
static void imxrt_gt9xx_clear(FAR const struct gt9xx_config_s *config)
{
  imxrt_gpioirq_disable(g_gt9xx_irq);
}
#endif

/****************************************************************************
 * Name: imxrt_gt9xx_wakeup
 *
 * Description:
 *   Issue WAKE interrupt to GT9XX to change the GT9XX from Hibernate to
 *   Active mode.
 *
 ****************************************************************************/

static void imxrt_gt9xx_wakeup(FAR const struct gt9xx_config_s *config)
{
  /* We do not have access to the WAKE pin in the implementation */
}

/****************************************************************************
 * Name: imxrt_gt9xx_nreset
 *
 * Description:
 *   Control the chip reset pin (active low)
 *
 ****************************************************************************/

static void imxrt_gt9xx_nreset(FAR const struct gt9xx_config_s *config,
                                bool nstate)
{
  imxrt_gpio_write(GPIO_GT9XX_CTRSTN, nstate);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK      (0x7U)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT     (0U)

#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(x)        (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK)
#define IOMUXC_SW_MUX_CTL_PAD_SION_MASK          (0x10U)
#define IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT         (4U)

#define IOMUXC_SW_MUX_CTL_PAD_SION(x)            (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_SION_MASK)

#define IOMUXC_GPIO_B1_14_GPIO2_IO30 0x401F81B4U, 0x5U, 0, 0, 0x401F83A4U

static inline void IOMUXC_SetPinMux(uint32_t muxRegister,
                                    uint32_t muxMode,
                                    uint32_t inputRegister,
                                    uint32_t inputDaisy,
                                    uint32_t configRegister,
                                    uint32_t inputOnfield)
{
    *((volatile uint32_t *)muxRegister) =
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(muxMode) | IOMUXC_SW_MUX_CTL_PAD_SION(inputOnfield);

    if (inputRegister)
    {
        *((volatile uint32_t *)inputRegister) = inputDaisy;
    }
}

static inline void IOMUXC_SetPinConfig(uint32_t muxRegister,
                                       uint32_t muxMode,
                                       uint32_t inputRegister,
                                       uint32_t inputDaisy,
                                       uint32_t configRegister,
                                       uint32_t configValue)
{
  if (configRegister)
  {
    *((volatile uint32_t *)configRegister) = configValue;
  }
}

void imxrt_config_gt9xx_pins(void)
{
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B1_14_GPIO2_IO30,          /* WAKEUP is configured as GPIO5_IO00 */
      0U);                                   /* Software Input On Field: Input Path is determined by functionality */

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B1_14_GPIO2_IO30,
      0x10B0u);
}

/****************************************************************************
 * Name: imxrt_gt9xx_register
 *
 * Description:
 *   Register the GT9XX touch panel driver
 *
 ****************************************************************************/

int imxrt_gt9xx_register(void)
{
  FAR struct i2c_master_s *i2c;
  int ret;

  /* Initialize CTRSTN pin */
  imxrt_config_gpio(GPIO_GT9XX_CTRSTN);
  imxrt_gpio_write(GPIO_GT9XX_CTRSTN, false);

#ifndef CONFIG_GT9XX_POLLMODE
  int irq;

  /* Initialize GPIO interrupt pin. */
  imxrt_config_gpio(GPIO_GT9XX_CTINTN);

  irq = GPIO_GT9XX_INTR;
  DEBUGASSERT(irq > 0 && irq < UINT8_MAX);
  g_gt9xx_irq = (uint8_t)irq;

  /* Make sure that the interrupt is disabled at the NVIC */
  imxrt_gpioirq_disable(irq);
  up_disable_irq(irq);
#endif

  /* Take the GT9XX out of reset */

  /* The GT9XX is on LPI2C1.  Get the handle and register the GT9XX device */

  i2c = imxrt_i2cbus_initialize(1);
  if (i2c == NULL)
    {
      syslog(LOG_ERR, "ERROR: Failed to get LPI2C1 interface\n");
      return -ENODEV;
    }
  else
    {
      ret = gt9xx_register(i2c, &g_gt9xx_config, 0);
      if (ret < 0)
        {
          syslog(LOG_ERR, "ERROR: Failed to register GT9XX driver: %d\n", ret);
          imxrt_gpio_write(GPIO_GT9XX_CTRSTN, false);
          imxrt_i2cbus_uninitialize(i2c);
          return ret;
        }
    }

  return OK;
}

#endif /* CONFIG_INPUT_GT9XX*/
