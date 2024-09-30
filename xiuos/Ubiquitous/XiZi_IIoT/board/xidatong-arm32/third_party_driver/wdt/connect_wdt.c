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
* @file connect_wdt.c
* @brief support imxrt1052-board watchdog(WDG1) function and register to bus framework
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-05-06
*/

#include <connect_wdt.h>
#include <fsl_wdog.h>

static BusType wdt;
static wdog_config_t wdog_config_t_param;

void WDOG1_IRQHandler(void)
{
    WDOG_ClearInterruptStatus(WDOG1, kWDOG_InterruptFlag);
    /* User code. User can do urgent case before timeout reset.
     * IE. user can backup the ram data or ram log to flash.
     * the period is set by config.interruptTimeValue, user need to
     * check the period between interrupt and timeout.
     */
}

static uint32 Imxrt1052WdgOpen(void *dev)
{
    return EOK;
}

static uint32 Imxrt1052WdgClose(void *dev)
{
    WDOG_Deinit(WDOG1);

    return EOK;
}

static int Imxrt1052WdgInit(struct WdtHardwareDevice *dev, uint16_t timeout)
{
    /*
        * wdogConfig->enableWdog = true;
        * wdogConfig->workMode.enableWait = true;
        * wdogConfig->workMode.enableStop = false;
        * wdogConfig->workMode.enableDebug = false;
        * wdogConfig->enableInterrupt = false;
        * wdogConfig->enablePowerdown = false;
        * wdogConfig->resetExtension = flase;
        * wdogConfig->timeoutValue = 0xFFU;
        * wdogConfig->interruptTimeValue = 0x04u;
        */
        WDOG_GetDefaultConfig(&wdog_config_t_param);
        wdog_config_t_param.timeoutValue = timeout; /* Timeout value is 1 sec / 6.4 num, 5s means 32. */
        WDOG_Init(WDOG1, &wdog_config_t_param);

    return EOK;
}

static uint32 Imxrt1052WdgConfigure(void *drv, struct BusConfigureInfo *args)
{
    struct WdtDriver *wdt_drv = (struct WdtDriver *)drv;
    struct WdtHardwareDevice *wdt_dev = (struct WdtHardwareDevice *)wdt_drv->driver.owner_bus->owner_haldev;
    
    uint16_t timeout;
    
    switch(args->configure_cmd)
    {
        case OPER_WDT_SET_TIMEOUT:
            timeout = *(uint16_t *)(args->private_data);
            if (timeout) {
                Imxrt1052WdgInit(wdt_dev, timeout);
            }
            break;
        case OPER_WDT_KEEPALIVE:
            WDOG_Refresh(WDOG1);
            break;
        default:
            return ERROR;
    }
    return EOK;
}

static const struct WdtDevDone dev_done =
{
    Imxrt1052WdgOpen,
    Imxrt1052WdgClose,
    NONE,
    NONE,
};

/**
 * @description: Feed watchdog task function
 */
static void FeedWatchdogTask(void)
{
    while (1)
    {
        /* keep watchdog alive in idle task */
        struct BusConfigureInfo cfg;
        cfg.configure_cmd = OPER_WDT_KEEPALIVE;
        cfg.private_data = NONE;
        BusDrvConfigure(wdt->owner_driver, &cfg);
        MdelayKTask(500);
    }
}

/**
 * @description: Watchdog function
 * @return success: EOK, failure: other
 */
int StartWatchdog(void)
{
    int ret = EOK;
    uint16_t timeout = 32;    /* timeout time 5s*/

    wdt = BusFind(WDT_BUS_NAME);
    wdt->owner_driver = BusFindDriver(wdt, WDT_DRIVER_NAME);

    /* set watchdog timeout time */
    struct BusConfigureInfo cfg;
    cfg.configure_cmd = OPER_WDT_SET_TIMEOUT;
    cfg.private_data = &timeout;
    ret = BusDrvConfigure(wdt->owner_driver, &cfg);
    
    int32 WdtTask = KTaskCreate("WdtTask", (void *)FeedWatchdogTask, NONE, 1024, 20); 
	StartupKTask(WdtTask);

    return EOK;
}

int Imxrt1052HwWdgInit(void)
{
    x_err_t ret = EOK;

    static struct WdtBus watch_dog_timer_bus;
    static struct WdtDriver watch_dog_timer_drv;
    static struct WdtHardwareDevice watch_dog_timer_dev;

    ret = WdtBusInit(&watch_dog_timer_bus, WDT_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Watchdog bus init error %d\n", ret);
        return ERROR;
    }

    watch_dog_timer_drv.configure = Imxrt1052WdgConfigure;
    ret = WdtDriverInit(&watch_dog_timer_drv, WDT_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("Watchdog driver init error %d\n", ret);
        return ERROR;
    }

    ret = WdtDriverAttachToBus(WDT_DRIVER_NAME, WDT_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Watchdog driver attach error %d\n", ret);
        return ERROR;
    }

    watch_dog_timer_dev.dev_done = &dev_done;

    ret = WdtDeviceRegister(&watch_dog_timer_dev, WDT_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("Watchdog device register error %d\n", ret);
        return ERROR;
    }
    ret = WdtDeviceAttachToBus(WDT_DEVICE_NAME, WDT_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Watchdog device register error %d\n", ret);
        return ERROR;
    }

    return ret;
}
