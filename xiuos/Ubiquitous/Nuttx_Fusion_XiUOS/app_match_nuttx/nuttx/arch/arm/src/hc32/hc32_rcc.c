/****************************************************************************
 * arch/arm/src/hc32/hc32_rcc.c
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

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <debug.h>

#include <arch/board/board.h>

#include "arm_internal.h"
#include "chip.h"
#include "hc32_rcc.h"

#include "hc32f4a0_clk.h"
#include "hc32f4a0_sram.h"
#include "hc32f4a0_efm.h"
#include "hc32f4a0_pwc.h"
#include "hc32f4a0_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Allow up to 100 milliseconds for the high speed clock to become ready.
 * that is a very long delay, but if the clock does not become ready we are
 * hosed anyway.
 */

#define HSERDY_TIMEOUT (100 * CONFIG_BOARD_LOOPSPERMSEC)

/*!< System clock frequency (Core clock) */
__NO_INIT uint32_t SystemCoreClock;
/*!< High speed RC frequency (HCR clock) */
__NO_INIT uint32_t HRC_VALUE;

/****************************************************************************
 * Included Files
 ****************************************************************************/

/* Include chip-specific clocking initialization logic */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief  Setup the microcontroller system. Initialize the System and update
 *         the SystemCoreClock variable.
 * @param  None
 * @retval None
 */
void hc32_sysclk_init(void)
{
    /* FPU settings */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
    hc32_sysclk_update();
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 * @param  None
 * @retval None
 */
void hc32_sysclk_update(void)
{
    uint8_t tmp;
    uint32_t plln;
    uint32_t pllp;
    uint32_t pllm;

    /* Select proper HRC_VALUE according to ICG1.HRCFREQSEL bit */
    /* ICG1.HRCFREQSEL = '0' represent HRC_VALUE = 20000000UL   */
    /* ICG1.HRCFREQSEL = '1' represent HRC_VALUE = 16000000UL   */
    if (1UL == (HRC_FREQ_MON() & 1UL))
    {
        HRC_VALUE = HRC_16MHz_VALUE;
    }
    else
    {
        HRC_VALUE = HRC_20MHz_VALUE;
    }

    tmp = M4_CMU->CKSWR & CMU_CKSWR_CKSW;
    switch(tmp)
    {
        case 0x00U:  /* use internal high speed RC */
            SystemCoreClock = HRC_VALUE;
            break;
        case 0x01U:  /* use internal middle speed RC */
            SystemCoreClock = MRC_VALUE;
            break;
        case 0x02U:  /* use internal low speed RC */
            SystemCoreClock = LRC_VALUE;
            break;
        case 0x03U:  /* use external high speed OSC */
            SystemCoreClock = XTAL_VALUE;
            break;
        case 0x04U:  /* use external low speed OSC */
            SystemCoreClock = XTAL32_VALUE;
            break;
        case 0x05U:  /* use PLLH */
            /* PLLCLK = ((pllsrc / pllm) * plln) / pllp */
            pllp = (uint32_t)((M4_CMU->PLLHCFGR >> CMU_PLLHCFGR_PLLHP_POS) & 0x0FUL);
            plln = (uint32_t)((M4_CMU->PLLHCFGR >> CMU_PLLHCFGR_PLLHN_POS) & 0xFFUL);
            pllm = (uint32_t)((M4_CMU->PLLHCFGR >> CMU_PLLHCFGR_PLLHM_POS) & 0x03UL);

            /* use external high speed OSC as PLL source */
            if (0UL == bM4_CMU->PLLHCFGR_b.PLLSRC)
            {
                SystemCoreClock = (XTAL_VALUE) / (pllm + 1UL) * (plln + 1UL) / (pllp + 1UL);
            }
            /* use internal high RC as PLL source */
            else
            {
                SystemCoreClock = (HRC_VALUE) / (pllm + 1UL) * (plln + 1UL) / (pllp + 1UL);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief  BSP clock initialize.
 *         Set board system clock to PLLH@240MHz
 *         Flash: 5 wait
 *         SRAM_HS: 1 wait
 *         SRAM1_2_3_4_B: 2 wait
 *         PCLK0: 240MHz
 *         PCLK1: 120MHz
 *         PCLK2: 60MHz
 *         PCLK3: 60MHz
 *         PCLK4: 120MHz
 *         EXCLK: 120MHz
 *         HCLK:  240MHz
 * @param  None
 * @retval None
 */
void hc32_clk_config(void)
{
    stc_clk_xtal_init_t stcXtalInit;
    stc_clk_pllh_init_t stcPLLHInit;

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                                   \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |             \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |             \
                CLK_HCLK_DIV1));

    CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8XtalMode = CLK_XTALMODE_OSC;
    stcXtalInit.u8XtalDrv = CLK_XTALDRV_LOW;
    stcXtalInit.u8XtalState = CLK_XTAL_ON;
    stcXtalInit.u8XtalStb = CLK_XTALSTB_2MS;
    CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLHStructInit(&stcPLLHInit);
    /* VCO = (8/1)*120 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 120UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL;
    (void)CLK_PLLHInit(&stcPLLHInit);

    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_1, SRAM_WAIT_CYCLE_1);

    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);

    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
}

/****************************************************************************
 * Name: rcc_resetbkp
 *
 * Description:
 *   The RTC needs to reset the Backup Domain to change RTCSEL and resetting
 *   the Backup Domain renders to disabling the LSE as consequence.
 *   In order to avoid resetting the Backup Domain when we already
 *   configured LSE we will reset the Backup Domain early (here).
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#if defined(CONFIG_HC32_RTC) && defined(CONFIG_HC32_PWR)
static inline void rcc_resetbkp(void)
{
  uint32_t regval;

  /* Check if the RTC is already configured */

//  hc32_pwr_initbkp(false);

  regval = getreg32(RTC_MAGIC_REG);
  if (regval != RTC_MAGIC && regval != RTC_MAGIC_TIME_SET)
    {
      hc32_pwr_enablebkp(true);

      /* We might be changing RTCSEL - to ensure such changes work, we must
       * reset the backup domain (having backed up the RTC_MAGIC token)
       */

      modifyreg32(HC32_RCC_XXX, 0, RCC_XXX_YYYRST);
      modifyreg32(HC32_RCC_XXX, RCC_XXX_YYYRST, 0);

      hc32_pwr_enablebkp(false);
    }
}
#else
#  define rcc_resetbkp()
#endif

/****************************************************************************
 * Name: rcc_reset
 *
 * Description:
 *   Reset the RCC clock configuration to the default reset state
 *
 ****************************************************************************/

static inline void rcc_reset(void)
{
}

static inline void rcc_enableperipherals(void)
{
}

#ifndef CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG
static void hc32_stdclockconfig(void)
{
}
#endif

void hc32_clk_init(void)
{
    hc32_sysclk_init();

    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
//    PWC_FCG0_Unlock();
//    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
//    PWC_Unlock(PWC_UNLOCK_CODE_0);
    /* Unlock SRAM register: WTCR */
    SRAM_WTCR_Unlock();
    /* Unlock SRAM register: CKCR */
    // SRAM_CKCR_Unlock();
    /* Unlock all EFM registers */
    EFM_Unlock();

    hc32_clk_config();

    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0);
    /* Lock SRAM register: WTCR */
    SRAM_WTCR_Lock();
    /* Lock SRAM register: CKCR */
    // SRAM_CKCR_Lock();
    /* Lock EFM OTP write protect registers */
    // EFM_OTP_WP_Lock();
    /* Lock EFM register: FWMC */
    // EFM_FWMC_Lock();
    /* Lock all EFM registers */
    EFM_Lock();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: hc32_clockconfig
 *
 * Description:
 *   Called to establish the clock settings based on the values in board.h.
 *   This function (by default) will reset most everything, enable the PLL,
 *   and enable peripheral clocking for all peripherals enabled in the NuttX
 *   configuration file.
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

void hc32_clockconfig(void)
{
  /* Make sure that we are starting in the reset state */

  rcc_reset();

  /* Reset backup domain if appropriate */

  rcc_resetbkp();

#if defined(CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG)

  /* Invoke Board Custom Clock Configuration */

  hc32_board_clockconfig();

#else

  /* Invoke standard, fixed clock configuration based on definitions
   * in board.h
   */

  hc32_stdclockconfig();

#endif

  /* Enable peripheral clocking */

  rcc_enableperipherals();

#ifdef CONFIG_HC32_SYSCFG_IOCOMPENSATION
  /* Enable I/O Compensation */

  hc32_iocompensation();
#endif

  hc32_clk_init();
}

/****************************************************************************
 * Name: hc32_clockenable
 *
 * Description:
 *   Re-enable the clock and restore the clock settings based on settings
 *   in board.h. This function is only available to support low-power
 *   modes of operation:  When re-awakening from deep-sleep modes, it is
 *   necessary to re-enable/re-start the PLL
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
void hc32_clockenable(void)
{
#if defined(CONFIG_ARCH_BOARD_HC32_CUSTOM_CLOCKCONFIG)

  /* Invoke Board Custom Clock Configuration */

  hc32_board_clockconfig();

#else

  /* Invoke standard, fixed clock configuration based on definitions
   * in board.h
   */

  hc32_stdclockconfig();

#endif
}
#endif
