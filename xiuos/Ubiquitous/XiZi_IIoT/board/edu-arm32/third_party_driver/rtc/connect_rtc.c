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
* @file connect_rtc.c
* @brief support aiit-edu-arm32-board rtc function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2023-02-02
*/

#include <connect_rtc.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static uint32 RtcConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);

    struct RtcDriver *rtc_drv = (struct RtcDriver *)drv;
    struct RtcDrvConfigureParam *drv_param = (struct RtcDrvConfigureParam *)configure_info->private_data;

    int cmd = drv_param->rtc_operation_cmd;
    time_t *time = drv_param->time;

    switch (cmd)
    {
        case OPER_RTC_GET_TIME:
        {
            struct tm ct;
            stc_rtc_date_t rtc_date;
            stc_rtc_time_t rtc_time;

            // rtc_timer_get(&year, &month, &day, &hour, &minute, &second);
            RTC_GetDate(RTC_DATA_FMT_DEC, &rtc_date);
            RTC_GetTime(RTC_DATA_FMT_DEC, &rtc_time);

            ct.tm_year = rtc_date.u8Year ;
            ct.tm_mon = rtc_date.u8Month ;
            ct.tm_mday = rtc_date.u8Day;
            ct.tm_wday = rtc_date.u8Weekday;

            ct.tm_hour = rtc_time.u8Hour;
            ct.tm_min = rtc_time.u8Minute;
            ct.tm_sec = rtc_time.u8Second;

            *time = mktime(&ct);
        }
        break;
        case OPER_RTC_SET_TIME:
        {
            struct tm *ct;
            stc_rtc_date_t rtc_date;
            stc_rtc_time_t rtc_time;
            x_base lock;

            lock = CriticalAreaLock();
            ct = localtime(time);
            rtc_date.u8Year = ct->tm_year ;
            rtc_date.u8Month = ct->tm_mon ;
            rtc_date.u8Day = ct->tm_mday;
            rtc_date.u8Weekday = ct->tm_wday;
            rtc_time.u8Hour = ct->tm_hour;
            rtc_time.u8Minute = ct->tm_min;
            rtc_time.u8Second = ct->tm_sec;
            CriticalAreaUnLock(lock);

            RTC_SetDate(RTC_DATA_FMT_DEC, &rtc_date);
            RTC_SetTime(RTC_DATA_FMT_DEC, &rtc_time);
        }
        break;
    }
    return EOK;
}

/*manage the rtc device operations*/
static const struct RtcDevDone dev_done =
{
    .open = NONE,
    .close = NONE,
    .write = NONE,
    .read = NONE,
};

static int BoardRtcBusInit(struct RtcBus *rtc_bus, struct RtcDriver *rtc_driver)
{
    x_err_t ret = EOK;

    /*Init the rtc bus */
    ret = RtcBusInit(rtc_bus, RTC_BUS_NAME);
    if (EOK != ret) {
        KPrintf("HwRtcInit RtcBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the rtc driver*/
    ret = RtcDriverInit(rtc_driver, RTC_DRV_NAME);
    if (EOK != ret) {
        KPrintf("HwRtcInit RtcDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the rtc driver to the rtc bus*/
    ret = RtcDriverAttachToBus(RTC_DRV_NAME, RTC_BUS_NAME);
    if (EOK != ret) {
        KPrintf("HwRtcInit RtcDriverAttachToBus error %d\n", ret);
        return ERROR;
    } 

    return ret;
}

/*Attach the rtc device to the rtc bus*/
static int BoardRtcDevBend(void)
{
    x_err_t ret = EOK;

    static struct RtcHardwareDevice rtc_device;
    memset(&rtc_device, 0, sizeof(struct RtcHardwareDevice));

    rtc_device.dev_done = &(dev_done);

    ret = RtcDeviceRegister(&rtc_device, NONE, RTC_DEVICE_NAME);
    if (EOK != ret) {
        KPrintf("HwRtcInit RtcDeviceInit device %s error %d\n", RTC_DEVICE_NAME, ret);
        return ERROR;
    }  

    ret = RtcDeviceAttachToBus(RTC_DEVICE_NAME, RTC_BUS_NAME);
    if (EOK != ret) {
        KPrintf("HwRtcInit RtcDeviceAttachToBus device %s error %d\n", RTC_DEVICE_NAME, ret);
        return ERROR;
    }  

    return  ret;
}


int HwRtcInit(void)
{
    x_err_t ret = EOK;

    static struct RtcBus rtc_bus;
    memset(&rtc_bus, 0, sizeof(struct RtcBus));

    static struct RtcDriver rtc_driver;
    memset(&rtc_driver, 0, sizeof(struct RtcDriver));
    
    rtc_driver.configure = &(RtcConfigure);

    ret = BoardRtcBusInit(&rtc_bus, &rtc_driver);
    if (EOK != ret) {
        KPrintf("HwRtcInit error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardRtcDevBend();
    if (EOK != ret) {
        KPrintf("HwRtcInit error ret %u\n", ret);
    }
    
    stc_rtc_init_t stcRtcInit;
    /* Configure structure initialization */
    (void)RTC_StructInit(&stcRtcInit);

    /* Configuration RTC structure */
    stcRtcInit.u8ClockSrc  = RTC_CLK_SRC_XTAL32;
    stcRtcInit.u8HourFormat= RTC_HOUR_FMT_24H;
    stcRtcInit.u8IntPeriod = RTC_INT_PERIOD_PER_SEC;
    (void)RTC_Init(&stcRtcInit);

    RTC_Cmd(LL_RTC_ENABLE);

    return ret;
}
