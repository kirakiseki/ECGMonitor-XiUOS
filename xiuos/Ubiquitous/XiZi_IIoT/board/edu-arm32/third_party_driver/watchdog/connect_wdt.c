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
* @brief support edu-arm32-board watchdog function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2023-02-02
*/

#include <connect_wdt.h>

#define WDT_COUNT_CYCLE 65536U

static uint32 WdtOpen(void *dev)
{
    NULL_PARAM_CHECK(dev);

    stc_wdt_init_t stcWdtInit;
    stcWdtInit.u32CountPeriod   = WDT_CNT_PERIOD65536;
    stcWdtInit.u32ClockDiv      = WDT_CLK_DIV1024;
    stcWdtInit.u32RefreshRange  = WDT_RANGE_0TO25PCT;
    stcWdtInit.u32LPMCount      = WDT_LPM_CNT_STOP;
    stcWdtInit.u32ExceptionType = WDT_EXP_TYPE_RST;
    (void)WDT_Init(&stcWdtInit);
    return EOK;
}

static uint32 WdtConfigure(void *drv, struct BusConfigureInfo *args)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(args);

    stc_wdt_init_t stcWdtInit;

    int period_option = *((int*)args->private_data);
    if(period_option<=256){
        period_option = WDT_CNT_PERIOD256;
    }else if(period_option<=4096){
        period_option = WDT_CNT_PERIOD4096;
    }else if(period_option<=16384){
        period_option = WDT_CNT_PERIOD16384;
    }else{
        period_option = WDT_CNT_PERIOD65536;
    }

    switch (args->configure_cmd)
    {
        case OPER_WDT_SET_TIMEOUT:
        stcWdtInit.u32CountPeriod    = period_option;
        stcWdtInit.u32ClockDiv       = WDT_CLK_DIV1024;
        stcWdtInit.u32RefreshRange   = WDT_RANGE_0TO25PCT;
        stcWdtInit.u32LPMCount       = WDT_LPM_CNT_STOP;
        stcWdtInit.u32ExceptionType  = WDT_EXP_TYPE_RST;
        if (WDT_Init(&stcWdtInit) != 0) {
            return ERROR;
        }
        /* the chip SDK's feature:to start up watchdog counter, feed dog first after initialization*/
        WDT_FeedDog();
        break;
        case OPER_WDT_KEEPALIVE:
            /* must wait for count lower than 25%(division by 4) for a feed as RefreshRange is set as 0TO25PCT*/
            if (WDT_GetCountValue() < WDT_COUNT_CYCLE/4){  
                WDT_FeedDog();
            }
            break;
        default:
            return ERROR;
    }
    return EOK;
}

static const struct WdtDevDone dev_done =
{
    WdtOpen,
    NONE,
    NONE,
    NONE,
};

/**
 * @description: Watchdog function
 * @return success: EOK, failure: other
 */
int StartWatchdog(void)
{
    //add feed watchdog task function

    return EOK;
}

int HwWdtInit(void)
{
    x_err_t ret = EOK;

    static struct WdtBus wdt0;

    ret = WdtBusInit(&wdt0, WDT_BUS_NAME_0);
    if (ret != EOK) {
        KPrintf("Watchdog bus init error %d\n", ret);
        return ERROR;
    }

    static struct WdtDriver drv0;
    drv0.configure = WdtConfigure;
    
    ret = WdtDriverInit(&drv0, WDT_DRIVER_NAME_0);
    if (ret != EOK) {
        KPrintf("Watchdog driver init error %d\n", ret);
        return ERROR;
    }

    ret = WdtDriverAttachToBus(WDT_DRIVER_NAME_0, WDT_BUS_NAME_0);
    if (ret != EOK) {
        KPrintf("Watchdog driver attach error %d\n", ret);
        return ERROR;
    }

    static struct WdtHardwareDevice dev0;
    dev0.dev_done = &dev_done;

    ret = WdtDeviceRegister(&dev0, WDT_0_DEVICE_NAME_0);
    if (ret != EOK) {
        KPrintf("Watchdog device register error %d\n", ret);
        return ERROR;
    }

    ret = WdtDeviceAttachToBus(WDT_0_DEVICE_NAME_0, WDT_BUS_NAME_0);
    if (ret != EOK) {
        KPrintf("Watchdog device register error %d\n", ret);
        return ERROR;
    }

    return ret;
}
