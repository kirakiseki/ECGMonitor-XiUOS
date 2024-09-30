/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
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
* @brief support ch32v307 init configure and start-up
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-08-08
*/
#include <board.h>
#include <xizi.h>
#include <stdint.h>
#include <device.h>
#include "connect_uart.h"
#include "xsconfig.h"
#include "ch32v30x.h"
#include "core_riscv.h"
 // #include <system_ch32v30x.h>

 // core clock.
extern uint32_t SystemCoreClock;

static uint32_t _SysTick_Config(uint32_t ticks)
{
    NVIC_SetPriority(SysTicK_IRQn,0xf0);
    NVIC_SetPriority(Software_IRQn,0xf0);
    NVIC_EnableIRQ(SysTicK_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
    SysTick->CTLR=0;
    SysTick->SR=0;
    SysTick->CNT=0;
    SysTick->CMP=ticks-1;
    SysTick->CTLR=0xF;
    return 0;
}

/**
 * This function will initial your board.
 */
void InitBoardHardware()
{
    USART_Printf_Init(115200);
    /* System Tick Configuration */
    _SysTick_Config(SystemCoreClock / TICK_PER_SECOND);
    /* initialize memory system */
    InitBoardMemory(MEMORY_START_ADDRESS, (void*)MEMORY_END_ADDRESS);
    InitHwUart();
    InstallConsole("uart1", "uart1_drv", "uart1_dev1");

    KPrintf("consle init completed.\n");
    KPrintf("board initialization......\n");
    // KPrintf("memory address range: [0x%08x - 0x%08x], size: %d\n", (x_ubase) MEMORY_START_ADDRESS, (x_ubase) MEMORY_END_ADDRESS, gd32vf103_SRAM_SIZE);
    /* initialize memory system */
	
    KPrintf("board init done.\n");
	KPrintf("start okernel...\n");
}

