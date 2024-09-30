/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* @file mucboot.c
* @brief support bootloader function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-04-03
*/

#include <stdint.h>
#include <xs_base.h>
#include "common.h"
#include "mcuboot.h"
#include "flash.h"

#ifdef TOOL_USING_OTA

void mcuboot_bord_init(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    UartConfig();
    SysTick_Config(SystemCoreClock / TICK_PER_SECOND);
}

void mcuboot_reset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

void mcuboot_jump(void)
{
    uint32_t addr = XIUOS_FLAH_ADDRESS;

    SCB->VTOR = addr;
    asm volatile("LDR R0, %0" : : "m"(addr));
    asm volatile("LDR   R0, [R0]");
    asm volatile("MOV   SP, R0");
    
    addr += 4;
    asm volatile("LDR R0, %0" : : "m"(addr));
    asm volatile("LDR   R0, [R0]");
    asm volatile("BX  R0");
}

extern void ImxrtMsDelay(uint32 ms);

void mcuboot_delay(uint32_t ms)
{
    ImxrtMsDelay(ms);
}
#endif