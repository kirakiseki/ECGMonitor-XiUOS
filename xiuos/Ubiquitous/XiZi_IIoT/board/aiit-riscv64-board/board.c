/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
* @file board.c
* @brief support edu-riscv64 board init configure and start-up
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-10-25
*/

/*************************************************
File name: board.c
Description: support edu-riscv64 board init configure and driver/task/... init
Others: https://canaan-creative.com/developer
History: 
1. Date: 2022-10-25
Author: AIIT XUOS Lab
Modification: 
1. support edu-riscv64 board InitBoardHardware
2. support edu-riscv64 board Kd233Start
3. support edu-riscv64 board shell cmd, include reboot, shutdown
*************************************************/

#include <board.h>
#include <dmac.h>
#include <clint.h>
#include <connect_gpio.h>
#include <connect_uart.h>
#include <encoding.h>
#include <fpioa.h>
#include <sysctl.h>
#include <tick.h>

#if defined(FS_VFS)
#include <iot-vfs.h>
#endif

#define CPU0    (0)
#define CPU1    (1)
extern x_base cpu2_boot_flag;
extern void entry(void);
extern void SecondaryCpuCStart(void);
extern int IoConfigInit(void);
extern int HwCh438Init(void);
extern int HwSpiInit(void);
extern int HwLcdInit(void);
extern int HwWdtInit(void);
extern int HwI2cInit(void);
extern int HwTimerInit(void);
extern int HwRtcInit(void);
extern int HwTouchBusInit(void);
extern int HwCh376Init(void);

#ifdef FS_CH376
#include <iot-vfs.h>
#ifdef MOUNT_USB
/**
 * @description: Mount USB
 * @return 0
 */
int  MountUsb(void)
{
    if (MountFilesystem(USB_BUS_NAME, USB_DEVICE_NAME, USB_DRIVER_NAME, FSTYPE_CH376, "/") == 0)
        KPrintf("usb mount to '/'");
    else
        KPrintf("usb mount to '/' failed!");
    
    return 0;
}
#endif
#ifdef MOUNT_SDCARD
/**
 * @description: Mount SD card
 * @return 0
 */
int  MountSDCard(void)
{
    if (MountFilesystem(SDIO_BUS_NAME, SDIO_DEVICE_NAME, SDIO_DRIVER_NAME, FSTYPE_CH376, "/") == 0)
        DBG("sd card mount to '/'");
    else
        SYS_WARN("sd card mount to '/' failed!");
    
    return 0;
}
#endif
#endif

void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

void Kd233Start(uint32_t mhartid)
{
	switch(mhartid) {
		case CPU0:
    		init_bss();

			/*kernel start entry*/
    		entry();
			break;
		case CPU1:
			while(0x2018050420191010 != cpu2_boot_flag) { ///< waiting for boot flag ,then start cpu1 core
#ifndef ARCH_SMP
				asm volatile("wfi");
#endif
			}
#ifdef ARCH_SMP
			SecondaryCpuCStart();
#endif
			break;

		default:
			break;
	}
}

int Freq(void)
{
    uint64 value = 0;

    value = SysctlClockGetFreq(SYSCTL_CLOCK_PLL0);
    KPrintf("PLL0: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_PLL1);
    KPrintf("PLL1: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_PLL2);
    KPrintf("PLL2: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_CPU);
    KPrintf("CPU : %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_APB0);
    KPrintf("APB0: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_APB1);
    KPrintf("APB1: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_APB2);
    KPrintf("APB2: %d\n", value);
    value = SysctlClockGetFreq(SYSCTL_CLOCK_HCLK);
    KPrintf("HCLK: %d\n", value);

    value = clint_get_time();
    KPrintf("mtime: %d\n", value);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0),Freq, Freq, show frequency information );

#ifdef ARCH_SMP
extern int EnableHwclintIpi(void);
#endif

struct InitSequenceDesc _board_init[] = 
{
#ifdef BSP_USING_GPIO
    { "hw_pin", HwGpioInit },
	{ "io_config", IoConfigInit },
#endif
#ifdef BSP_USING_CH438
    { "hw_extuart", HwCh438Init },
#endif
#ifdef BSP_USING_SPI
	{ "hw_spi", HwSpiInit },
#endif
#ifdef BSP_USING_I2C 
    { "hw_i2c", HwI2cInit },
#endif
#ifdef BSP_USING_RTC
    { "hw_rtc", HwRtcInit },
#endif
#ifdef BSP_USING_HWTIMER
    { "hw_timer" , HwTimerInit },
#endif
#ifdef BSP_USING_LCD
	{ "hw_lcd", HwLcdInit },
#endif
#ifdef BSP_USING_WDT
    { "hw_wdt", HwWdtInit },
#endif
#ifdef BSP_USING_SDIO
    { "hw_sdio", HwCh376Init},
#endif
#ifdef BSP_USING_USB
    { "hw_usb", HwCh376Init},
#endif
#ifdef BSP_USING_TOUCH
    { "hw_touch", HwTouchBusInit},
#endif
	{ " NONE ",NONE },
};

void InitBoardHardware(void)
{
	int i = 0;
	int ret = 0;
	
    SysctlPllSetFreq(SYSCTL_PLL0, 800000000UL);
    SysctlPllSetFreq(SYSCTL_PLL1, 400000000UL);
#ifdef BSP_USING_GPIO
    /* Init FPIOA */
    FpioaInit();
#endif
#ifdef BSP_USING_DMA
    /* Dmac init */
    DmacInit();
#endif

    /* initialize memory system */
	InitBoardMemory(MEMORY_START_ADDRESS, MEMORY_END_ADDRESS);

    /* initalize interrupt */
    InitHwinterrupt();
#ifdef BSP_USING_UART    
    HwUartInit();
#endif
#ifdef KERNEL_CONSOLE
    /* set console device */
    InstallConsole(KERNEL_CONSOLE_BUS_NAME, KERNEL_CONSOLE_DRV_NAME, KERNEL_CONSOLE_DEVICE_NAME);
	KPrintf("\nconsole init completed.\n");
    KPrintf("board initialization......\n");
#endif /* KERNEL_CONSOLE */

    InitHwTick();

#ifdef ARCH_SMP
    EnableHwclintIpi();
#endif


#ifdef KERNEL_COMPONENTS_INIT
	for(i = 0; _board_init[i].fn != NONE; i++) {
		ret = _board_init[i].fn();
		KPrintf("initialize %s %s\n",_board_init[i].fn_name, ret == 0 ? "success" : "failed");
	}
#endif
	KPrintf("board init done.\n");
	KPrintf("start kernel...\n");
}
void HwCpuReset(void)
{
    sysctl->soft_reset.soft_reset = 1;
    while(RET_TRUE);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0), Reboot, HwCpuReset, reset machine );

 
