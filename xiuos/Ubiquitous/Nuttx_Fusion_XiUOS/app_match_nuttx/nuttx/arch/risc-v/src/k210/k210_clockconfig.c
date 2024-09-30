/****************************************************************************
 * arch/risc-v/src/k210/k210_clockconfig.c
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

/**
* @file k210_clockconfig.c
* @brief nuttx source code
*                https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-09-28
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <arch/board/board.h>

#include "riscv_internal.h"
#include "k210_clockconfig.h"
#include "k210_sysctl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define OSC_FREQ 26000000UL

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint32_t g_cpu_clock = 390000000;//416000000;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: k210_get_cpuclk
 ****************************************************************************/

uint32_t k210_get_cpuclk(void)
{
  return g_cpu_clock;
}

/****************************************************************************
 * Name: k210_get_pll0clk
 ****************************************************************************/

#ifndef CONFIG_K210_WITH_QEMU
uint32_t k210_get_pll0clk(void)
{
  uint32_t pll0;
  uint32_t nr;
  uint32_t nf;
  uint32_t od;

  pll0 = getreg32(K210_SYSCTL_PLL0);
  nr   = PLL_CLK_R(pll0)  + 1;
  nf   = PLL_CLK_F(pll0)  + 1;
  od   = PLL_CLK_OD(pll0) + 1;

  return OSC_FREQ / nr * nf / od;
}

uint32_t k210_get_pll1clk(void)
{
  uint32_t pll1;
  uint32_t nr;
  uint32_t nf;
  uint32_t od;

  pll1 = getreg32(K210_SYSCTL_PLL1);
  nr   = PLL_CLK_R(pll1)  + 1;
  nf   = PLL_CLK_F(pll1)  + 1;
  od   = PLL_CLK_OD(pll1) + 1;

  return OSC_FREQ / nr * nf / od;
}

uint32_t k210_get_pll2clk(void)
{
  uint32_t pll2;
  uint32_t nr;
  uint32_t nf;
  uint32_t od;

  pll2 = getreg32(K210_SYSCTL_PLL2);
  nr   = PLL_CLK_R(pll2)  + 1;
  nf   = PLL_CLK_F(pll2)  + 1;
  od   = PLL_CLK_OD(pll2) + 1;

  return OSC_FREQ / nr * nf / od;
}

#endif

/****************************************************************************
 * Name: k210_clockconfig
 ****************************************************************************/

void k210_clockconfig(void)
{
#ifndef CONFIG_K210_WITH_QEMU
  uint32_t clksel0;

  /* Obtain clock selector for ACLK */
  clksel0 = getreg32(K210_SYSCTL_CLKSEL0);

  if (1 == CLKSEL0_ACLK_SEL(clksel0))
    {
      /* PLL0 selected */
      g_cpu_clock = k210_get_pll0clk() / 2;
    }
  else
    {
      /* OSC selected */

      g_cpu_clock = OSC_FREQ;
    }
#endif
}

void k210_sysctl_init(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 400000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

    sysctl_clock_set_threshold(SYSCTL_THRESHOLD_APB1, 2);

    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);

    // for IO-27/28
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, SYSCTL_POWER_V33);
    // for IO-20~23
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, SYSCTL_POWER_V33);
}
