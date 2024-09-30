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
* @file connect_dac.c
* @brief support to register DAC pointer and function
* @version 2.0
* @author AIIT XUOS Lab
* @date 2023-02-09
*/

#include <connect_dac.h>

#define _DAC_CONS(string1, string2) string1##string2
#define DAC_CONS(string1, string2) _DAC_CONS(string1, string2)

#ifdef BSP_USING_DAC
#define DAC_GPIO      DAC_CONS(GPIO_Pin_, DAC_GPIO_NUM)
#endif
			  

static uint32 DacOpen(void *dev)
{
    struct DacHardwareDevice *dac_dev = (struct DacHardwareDevice *)dev;

    CM_DAC_TypeDef *DACx  = (CM_DAC_TypeDef *)dac_dev->private_data;

    stc_dac_init_t pstcDacInit;

    DAC_StructInit(&pstcDacInit);

    DAC_Init(DACx,DAC_CH1,&pstcDacInit);

    return EOK;
}

static uint32 DacClose(void *dev)
{

    struct DacHardwareDevice *dac_dev = (struct DacHardwareDevice *)dev;

    CM_DAC_TypeDef *DACx  = (CM_DAC_TypeDef *)dac_dev->private_data;

    DAC_DeInit(DACx);

    return EOK;
}


static uint32 DacRead(void *dev, struct BusBlockReadParam *read_param)
{
    struct DacHardwareDevice *dac_dev = (struct DacHardwareDevice *)dev;

    CM_DAC_TypeDef *DACx  = (CM_DAC_TypeDef *)dac_dev->private_data;

    uint16 dac_set_value = 0;

    dac_set_value = DAC_GetChConvertState(DACx,DAC_CH1);

   *(uint16 *)read_param->buffer = dac_set_value;
   read_param->read_length = 2;

   return read_param->read_length;
    return EOK;
}

static uint32 DacDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;

    struct DacDriver *dac_drv = (struct DacDriver *)drv;
    struct DacHardwareDevice *dac_dev = (struct DacHardwareDevice *)dac_drv->driver.owner_bus->owner_haldev;
    struct HwDac *dac_cfg = (struct HwDac *)dac_dev->haldev.private_data;

    switch (configure_info->configure_cmd)
    {
        case OPE_CFG:
            dac_cfg->digital_data = *(uint16 *)configure_info->private_data;
            // DAC_SetChannel1Data(DAC_Align_12b_R, dac_cfg->digital_data);//12 bits、R-Align data format, digital data
            DAC_SetChData(dac_cfg->DACx,DAC_CH1,dac_cfg->digital_data);
            break;
        default:
            break;
    }

    return ret;
}

static const struct DacDevDone dev_done =
{
    DacOpen,
    DacClose,
    NONE,
    DacRead,
};

int HwDacInit(void)
{
    x_err_t ret = EOK;

#ifdef BSP_USING_DAC
    static struct DacBus dac_bus;
    static struct DacDriver dac_drv;
    static struct DacHardwareDevice dac_dev;
    static struct HwDac dac_cfg;

    dac_drv.configure = DacDrvConfigure;

    ret = DacBusInit(&dac_bus, DAC_BUS_NAME);
    if (ret != EOK) {
        KPrintf("DAC bus init error %d\n", ret);
        return ERROR;
    }

    ret = DacDriverInit(&dac_drv, DAC_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("DAC driver init error %d\n", ret);
        return ERROR;
    }
    ret = DacDriverAttachToBus(DAC_DRIVER_NAME, DAC_BUS_NAME);
    if (ret != EOK) {
        KPrintf("DAC driver attach error %d\n", ret);
        return ERROR;
    }

    dac_dev.dac_dev_done = &dev_done;
    dac_cfg.DACx = CM_DAC1;
    dac_cfg.digital_data = 0;

    ret = DacDeviceRegister(&dac_dev, (void *)&dac_cfg, DAC_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("DAC device register error %d\n", ret);
        return ERROR;
    }
    ret = DacDeviceAttachToBus(DAC_DEVICE_NAME, DAC_BUS_NAME);
    if (ret != EOK) {
        KPrintf("DAC device register error %d\n", ret);
        return ERROR;
    }
#endif 

    return ret;
}
