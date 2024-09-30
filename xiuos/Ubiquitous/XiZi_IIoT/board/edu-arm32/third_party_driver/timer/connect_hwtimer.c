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
* @file connect_hwtimer.c
* @brief support edu-arm32-board hwtimer function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-25
*/

#include <connect_hwtimer.h>

#define TMR0_CMP_VAL 1000
#define TMR0x                          ((CM_TMR0_TypeDef *)CM_TMR0_1_BASE)
#define TMR0_CH_x                      (TMR0_CH_A)
#define INTSEL_REG                     ((uint32_t)(&CM_INTC->SEL0))
#define TIMER0_IRQn                    (18)


void (*callback_function)(void *) ;

static void Timer0Callback(int vector, void *param)
{
    TMR0_SetCountValue(TMR0x, TMR0_CH_x, 0);
    if (callback_function) {
        callback_function(param);
    }

}

static uint32 HwtimerOpen(void *dev)
{
    struct HwtimerHardwareDevice *hwtimer_dev = dev;
    stc_tmr0_init_t stcTmr0Init;
    /* Enable timer0 peripheral clock */
    FCG_Fcg2PeriphClockCmd(PWC_FCG2_TMR0_1, ENABLE);

    /* TIMER0 basetimer function initialize */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV128;        /* Config clock division */
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_INTERN_CLK;          /* Chose clock source */
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;            /* Timer0 compare mode */
    stcTmr0Init.u16CompareValue = TMR0_CMP_VAL;             /* Set compare register data */
    (void)TMR0_Init(TMR0x, TMR0_CH_x, &stcTmr0Init);

    // DelayKTask(1);
    // /* Set internal hardware capture source */
    // TMR0_SetTriggerSrc(EVT_PORT_EIRQ0);

    // DelayKTask(1);

    return EOK;
}

static uint32 HwtimerClose(void *dev)
{
    /* Timer0 interrupt function Disable */
    TMR0_IntCmd(TMR0x, TMR0_INT_CMP_A, DISABLE);
    return EOK;
}

/*manage the hwtimer device operations*/
static const struct HwtimerDevDone dev_done =
{
    .open = HwtimerOpen,
    .close = HwtimerClose,
    .write = NONE,
    .read = NONE,
};

static uint32 HwtimerDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    __IO uint32_t *INTC_SELx;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            INTC_SELx = (__IO uint32_t *)(INTSEL_REG+ 4U * (uint32_t)(TIMER0_IRQn));
            WRITE_REG32(*INTC_SELx, EVT_SRC_TMR0_1_CMP_A);
            callback_function = (void (*)(void *param))configure_info->private_data;
            isrManager.done->registerIrq(TIMER0_IRQn+16,Timer0Callback,NULL);   
            isrManager.done->enableIrq(TIMER0_IRQn);
            TMR0_IntCmd(TMR0x, TMR0_INT_CMP_A, ENABLE);
            break;
        case OPE_CFG:
            TMR0_ClearStatus(TMR0x, TMR0_FLAG_CMP_A);
            TMR0_SetCompareValue(TMR0x, TMR0_CH_x, *((int *)configure_info->private_data) );
            /* Timer0 interrupt function Enable */
            TMR0_SetCountValue(TMR0x, TMR0_CH_x, 0x0000); 
            TMR0_Start(TMR0x, TMR0_CH_x);
            break;
        default:
            break;
    }
    return ret;
}

/*Init hwtimer bus*/
static int BoardHwtimerBusInit(struct HwtimerBus *hwtimer_bus, struct HwtimerDriver *hwtimer_driver)
{
    x_err_t ret = EOK;

    /*Init the hwtimer bus */
    ret = HwtimerBusInit(hwtimer_bus, HWTIMER_BUS_NAME_0);
    if (EOK != ret) {
        KPrintf("board_hwtimer_init HwtimerBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the hwtimer driver*/
    hwtimer_driver->configure = &(HwtimerDrvConfigure);
    ret = HwtimerDriverInit(hwtimer_driver, HWTIMER_DRIVER_NAME_0);
    if (EOK != ret) {
        KPrintf("board_hwtimer_init HwtimerDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the hwtimer driver to the hwtimer bus*/
    ret = HwtimerDriverAttachToBus(HWTIMER_DRIVER_NAME_0, HWTIMER_BUS_NAME_0);
    if (EOK != ret) {
        KPrintf("board_hwtimer_init USEDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

/*Attach the hwtimer device to the hwtimer bus*/
static int BoardHwtimerDevBend(void)
{
    x_err_t ret = EOK;
    static struct HwtimerHardwareDevice hwtimer_device_0;
    memset(&hwtimer_device_0, 0, sizeof(struct HwtimerHardwareDevice));

    hwtimer_device_0.dev_done = &dev_done;

    ret = HwtimerDeviceRegister(&hwtimer_device_0, NONE, HWTIMER_0_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("BoardHwtimerDevBend HwtimerDeviceRegister device %s error %d\n", HWTIMER_0_DEVICE_NAME_0, ret);
        return ERROR;
    }

    ret = HwtimerDeviceAttachToBus(HWTIMER_0_DEVICE_NAME_0, HWTIMER_BUS_NAME_0);
    if (EOK != ret) {
        KPrintf("BoardHwtimerDevBend HwtimerDeviceAttachToBus device %s error %d\n", HWTIMER_0_DEVICE_NAME_0, ret);
        return ERROR;
    }

    return ret;
}

/*EDU-ARM32 BOARD HWTIMER INIT*/
int HwTimerInit(void)
{
    x_err_t ret = EOK;
    static struct HwtimerBus hwtimer_bus;
    memset(&hwtimer_bus, 0, sizeof(struct HwtimerBus));

    static struct HwtimerDriver hwtimer_driver;
    memset(&hwtimer_driver, 0, sizeof(struct HwtimerDriver));

    ret = BoardHwtimerBusInit(&hwtimer_bus, &hwtimer_driver);
    if (EOK != ret) {
        KPrintf("board_hwtimer_Init error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardHwtimerDevBend();
    if (EOK != ret) {
        KPrintf("board_hwtimer_Init error ret %u\n", ret);
        return ERROR;
    }

    return ret;
}
