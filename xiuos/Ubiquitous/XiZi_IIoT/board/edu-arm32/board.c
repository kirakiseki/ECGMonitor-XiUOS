/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
* @file board.c
* @brief support edu-arm32-board init configure and start-up
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-08
*/

/*************************************************
File name: board.c
Description: support edu-arm32-board init configure and driver/task/... init
Others: 
History: 
1. Date: 2022-09-08
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board InitBoardHardware
*************************************************/

#include <xizi.h>
#include <board.h>
#include <hc32_ll.h>
#include <connect_usart.h>

#ifdef BSP_USING_GPIO
#include <connect_gpio.h>
#endif

#ifdef BSP_USING_ADC
#include <connect_adc.h>
#endif

#ifdef BSP_USING_DAC
#include <connect_dac.h>
#endif

#ifdef BSP_USING_SDIO
#include <connect_sdio.h>
#endif

#ifdef BSP_USING_SPI
#include <connect_spi.h>
#endif

#ifdef BSP_USING_I2C
#include <connect_i2c.h>
#endif

#ifdef BSP_USING_USB
#include <connect_usb.h>
#endif

#ifdef BSP_USING_RTC
#include <connect_rtc.h>
#endif

#ifdef BSP_USING_WDT
#include <connect_wdt.h>
#endif

#ifdef BSP_USING_TIMER
#include <connect_hwtimer.h>
#endif

#ifdef BSP_USING_CAN
#include <connect_can.h>
#endif

#ifdef BSP_USING_LWIP
#include <connect_ethernet.h>
#endif

extern void entry(void);
extern int HwUsartInit();

/* Peripheral register WE/WP selection */
#define LL_PERIPH_SEL                   (LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | \
                                         LL_PERIPH_EFM | LL_PERIPH_SRAM)

void SystemClockConfig(void)
{
    stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

    CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
    (void)CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_ULOW;
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLStructInit(&stcPLLHInit);

    stcPLLHInit.u8PLLState      = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR         = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM  = 1UL - 1UL;

#ifdef BSP_USING_USB
    /* VCO = (8/1)*120 = 960MHz*/
    stcPLLHInit.PLLCFGR_f.PLLN  = 120UL - 1UL;
#else
    /* VCO = (8/1)*100 = 800MHz*/
    stcPLLHInit.PLLCFGR_f.PLLN  = 100UL - 1UL;
#endif
    stcPLLHInit.PLLCFGR_f.PLLP  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    (void)CLK_PLLInit(&stcPLLHInit);

#ifdef BSP_USING_USB
    /* Highspeed SRAM set to 0 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    /* SRAM1_2_3_4_backup set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
#else
    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE2, SRAM_WAIT_CYCLE2);
#endif
    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT4);
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
}

void PeripheralClockConfig(void)
{
#if defined(BSP_USING_CAN1)
    CLK_SetCANClockSrc(CLK_CAN1, CLK_CANCLK_SYSCLK_DIV6);
#endif
#if   defined(BSP_USING_CAN2)
    CLK_SetCANClockSrc(CLK_CAN2, CLK_CANCLK_SYSCLK_DIV6);
#endif

#if defined(BSP_USING_ADC)
    CLK_SetPeriClockSrc(CLK_PERIPHCLK_PCLK);
#endif
}

void SysTickConfiguration(void)
{
    stc_clock_freq_t stcClkFreq;
    uint32_t cnts;

    CLK_GetClockFreq(&stcClkFreq);

    cnts = (uint32_t)stcClkFreq.u32HclkFreq / TICK_PER_SECOND;

    SysTick_Config(cnts);
}

void SysTick_Handler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    TickAndTaskTimesliceUpdate();

    ENABLE_INTERRUPT(lock);
}

struct InitSequenceDesc _board_init[] = 
{	
#ifdef BSP_USING_GPIO
    { "hw_pin", HwGpioInit },
#endif
#ifdef BSP_USING_SDIO
	{ "sdio", HwSdioInit },
#endif
#ifdef BSP_USING_SPI
	{ "spi", HwSpiInit },
#endif
#ifdef BSP_USING_I2C
	{ "i2c", HwI2cInit },
#endif
#ifdef BSP_USING_ADC
    {"hw adc init", HwAdcInit},
#endif
#ifdef BSP_USING_DAC
    {"hw dac init", HwDacInit},
#endif
#ifdef BSP_USING_USB
	{ "usb", HwUsbHostInit },
#endif
#ifdef BSP_USING_RTC
	{ "rtc", HwRtcInit },
#endif
#ifdef BSP_USING_WDT
	{ "wdt", HwWdtInit },
#endif
#ifdef BSP_USING_TIMER
	{ "tmr", HwTimerInit },
#endif
#ifdef BSP_USING_CAN
	{ "can", HwCanInit },
#endif
#ifdef BSP_USING_LWIP
#endif
    { " NONE ", NONE },
};

void InitBoardHardware()
{
	int i = 0;
	int ret = 0;

    /* MCU Peripheral registers write unprotected */
    LL_PERIPH_WE(LL_PERIPH_SEL);

    SystemClockConfig();

    PeripheralClockConfig();

    SysTickConfiguration();

#ifdef BSP_USING_UART
	HwUsartInit();
#endif

    InitBoardMemory((void *)MEMORY_START_ADDRESS, (void *)MEMORY_END_ADDRESS);

#ifdef KERNEL_CONSOLE
    InstallConsole(KERNEL_CONSOLE_BUS_NAME, KERNEL_CONSOLE_DRV_NAME, KERNEL_CONSOLE_DEVICE_NAME);

    KPrintf("\nconsole init completed.\n");
    KPrintf("board initialization......\n");
#endif

	for(i = 0; _board_init[i].fn != NONE; i++) {
		ret = _board_init[i].fn();
		KPrintf("initialize %s %s\n", _board_init[i].fn_name, ret == 0 ? "success" : "failed");
	}
  
    KPrintf("board init done.\n");
	KPrintf("start kernel...\n");
}
