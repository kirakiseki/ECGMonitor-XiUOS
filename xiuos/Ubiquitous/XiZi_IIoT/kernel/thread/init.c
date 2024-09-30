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
* @file:    init.c
* @brief:   init file
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/3/15
*
*/

#include <xizi.h>
#include <xs_assign.h>
#include <xs_init.h>
#include <xs_spinlock.h>
#include <xs_workqueue.h>
#include <stdlib.h>
#include <board.h>

#ifdef BSP_USING_USB
#ifndef BSP_USING_CH376
#include "connect_usb.h"
#endif
#endif

#ifdef BSP_USING_QSPI_FLASH
#include "connect_flash.h"
extern int FlashW25qxxSpiDeviceInit(void);
#endif

#ifdef KERNEL_USER_MAIN
#ifndef MAIN_KTASK_STACK_SIZE
#define MAIN_KTASK_STACK_SIZE     2048
#endif
#ifndef MAIN_KTASK_PRIORITY
#define MAIN_KTASK_PRIORITY       (KTASK_PRIORITY_MAX / 3)
#endif
#endif

#ifdef BSP_USING_WDT
extern int StartWatchdog(void);
#endif



extern void CreateKServiceKTask(void);
extern int main(void);
void InitBoardHardware(void);
extern int hook_init(void);
int CplusplusSystemInit(void);

#ifdef KERNEL_COMPONENTS_INIT
#ifdef USER_APPLICATION
extern void CreateMainTask(void);
#ifdef SEPARATE_COMPILE
int InitUserspace(void);
#endif
#endif

#ifndef ENV_INIT_KTASK_STACK_SIZE
#define ENV_INIT_KTASK_STACK_SIZE 8192
#endif

struct InitSequenceDesc prev_cmpts_init[] = 
{
#ifdef FS_VFS
	{ "vfs", VfsInit },
#endif
#ifdef  LIB_CPLUSPLUS
	{ "cplusplus_system", CplusplusSystemInit },
#endif
#ifdef KERNEL_HOOK
	{ "hook", hook_init },
#endif
	{ " NONE ", NONE },
};
struct InitSequenceDesc device_init[] = 
{
#ifdef KERNEL_WORKQUEUE
	{ "work sys workqueue", WorkSysWorkQueueInit },
#endif

#ifdef ARCH_ARM 
#ifdef RESOURCES_SPI_SFUD
	{ "W25Qxx_spi", FlashW25qxxSpiDeviceInit},
#endif
#endif

#ifdef RESOURCES_SPI_LORA
	{"LORA_spi", LoraSx12xxSpiDeviceInit},
#endif
	{ " NONE ", NONE },
};
struct InitSequenceDesc components_init[] = 
{
#ifdef FS_VFS_FATFS
	{ "fatfs", FatfsInit },
#endif
#ifdef FS_CH376
	{ "ch376", Ch376fsInit },
#endif
#ifdef FS_LWEXT4
	{ "lwext4", Lwext4Init },
#endif
	{ "libc_system", LibcSystemInit },
#ifdef RTC_SYNC_USING_NTP
	{ "rtc_ntp_sync",RtcNtpSyncInit},
#endif
	{ " NONE ", NONE },            
};
struct InitSequenceDesc env_init[] = 
{
#ifdef MOUNT_SDCARD
	{ "MountSDCard", MountSDCard },
#endif
#ifdef MOUNT_USB
	{ "MountUsb", MountUsb },
#endif
#ifdef FS_VFS_MNTTABLE
	{ "DfsMountTable", DfsMountTable },
#endif
#ifdef TOOL_SHELL
	{ "letter-shell system", userShellInit },
#endif
	{ " NONE ", NONE },
};
struct InitSequenceDesc communication_init[] = 
{
	{ " NONE ", NONE },
};

/**
 * This function will init sub components
 * @parm sub_components components type
 * 
 */
void _InitSubCmpts(struct InitSequenceDesc sub_cmpts[])
{
	int i = 0;
	int ret = 0;
	for( i = 0; sub_cmpts[i].fn != NONE; i++ ) {
		ret = sub_cmpts[i].fn();
		KPrintf("initialize %s %s\n",sub_cmpts[i].fn_name, ret == 0 ? "success" : "failed");
	}
}

#ifdef KERNEL_COMPONENTS_INIT
void EnvInitKTask(void *parameter)
{
	x_base lock = 0;
	lock = DISABLE_INTERRUPT();
	_InitSubCmpts(prev_cmpts_init);
	_InitSubCmpts(device_init);
	_InitSubCmpts(components_init);
	_InitSubCmpts(env_init);
	ENABLE_INTERRUPT(lock);
	_InitSubCmpts(communication_init);
	 
#ifdef ARCH_SMP
    StartupSecondaryCpu();
#endif

#ifdef USER_APPLICATION
#ifdef SEPARATE_COMPILE
	if(InitUserspace() == EOK) {
		CreateMainTask();
	}	
#else
	CreateMainTask();
#endif
#endif
}
#endif

void CreateEnvInitTask(void)
{
    int32 env_init = 0;

    env_init = KTaskCreate("env_init", EnvInitKTask, NONE,
		ENV_INIT_KTASK_STACK_SIZE, KTASK_PRIORITY_MAX - 1);
	if (env_init < 0) {
		KPrintf("env_init create failed ...%s %d.\n",__FUNCTION__,__LINE__);
		return;
	}

	StartupKTask(env_init);
}
#endif   /* KERNEL_COMPONENTS_INIT */


/**
 * kernel startup function
 * 
 * 
 */
int XiUOSStartup(void)
{
	DISABLE_INTERRUPT();

#ifdef KERNEL_QUEUEMANAGE
	QueuemanagerDoneRegister();
#endif

#ifdef KERNEL_BANNER
    ShowBanner();
#endif

    SysInitOsAssign();

    CreateKServiceKTask();

#ifdef KERNEL_COMPONENTS_INIT
	CreateEnvInitTask();
#else
 
#ifdef TOOL_SHELL
    extern int userShellInit(void);
	userShellInit();
#endif

#ifdef USER_APPLICATION
extern void CreateMainTask(void);
#ifdef SEPARATE_COMPILE
extern int InitUserspace(void);

	if(InitUserspace() == EOK) {
		CreateMainTask();
	}	
#else
	CreateMainTask();
#endif
#endif

#endif

#ifdef ARCH_SMP
	HwLockSpinlock(&AssignSpinLock);
#endif

#ifdef BSP_USING_WDT
    StartWatchdog();
#endif

	StartupOsAssign();
    return 0;
}


/* system entry */
int entry(void)
{
	DISABLE_INTERRUPT();

	/* system irq table must be inited before initialization of Hardware irq  */
	SysInitIsrManager();

    InitBoardHardware();
    XiUOSStartup();
    return 0;
}
