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
* @file connect_adc.c
* @brief support to register ADC pointer and function
* @version 1.1
* @author AIIT XUOS Lab
* @date 2023-02-09
*/

#include <connect_adc.h>

#define _ADC_CONS(string1, string2) string1##string2
#define ADC_CONS(string1, string2) _ADC_CONS(string1, string2)

#ifdef BSP_USING_ADC1
#define ADC1_GPIO      ADC_CONS(GPIO_Pin_, ADC1_GPIO_NUM)
#endif

#ifdef BSP_USING_ADC2
#define ADC2_GPIO      ADC_CONS(GPIO_Pin_, ADC2_GPIO_NUM)
#endif

#ifdef BSP_USING_ADC3
#define ADC3_GPIO      ADC_CONS(GPIO_Pin_, ADC3_GPIO_NUM)
#endif

static int AdcUdelay(uint32 us)
{
    uint32 ticks;
    uint32 told, tnow, tcnt = 0;
    uint32 reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / TICK_PER_SECOND);
    told = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                return 0;
                break;
            }
        }
    }
}

static uint16 GetAdcAverageValue(CM_ADC_TypeDef *ADCx, uint8 channel, uint8 times)
{
	uint32 temp_val = 0;
	int i;

	for(i = 0;i < times;i ++) {
		temp_val += ADC_GetValue(ADCx, channel) & 0x0FFF;
        KPrintf("GetAdcAverageValue val %u\n", ADC_GetValue(ADCx, channel));
		AdcUdelay(5000);
	}
	return temp_val / times;
} 


static uint32 AdcOpen(void *dev)
{
    x_err_t ret = EOK;
    stc_adc_init_t stcAdcInit;
    ADC_StructInit(&stcAdcInit);
    struct AdcHardwareDevice* adc_dev = (struct AdcHardwareDevice*)dev;
    CM_ADC_TypeDef *ADCx= (CM_ADC_TypeDef *)adc_dev->private_data;
    ADC_Init((ADCx),&stcAdcInit);
    return ret;
}


static uint32 AdcClose(void *dev)
{
    // CM_ADC_TypeDef *adc_dev = (CM_ADC_TypeDef*)dev;
    struct AdcHardwareDevice* adc_dev = (struct AdcHardwareDevice*)dev;

    CM_ADC_TypeDef *ADCx= (CM_ADC_TypeDef *)adc_dev->private_data;
    
    ADC_DeInit(ADCx);
    
    return EOK;
}

static uint32 AdcRead(void *dev, struct BusBlockReadParam *read_param)
{
    struct AdcHardwareDevice *adc_dev = (struct AdcHardwareDevice *)dev;

    struct HwAdc *adc_cfg = (struct HwAdc *)adc_dev->haldev.private_data;

    uint16 adc_average_value = 0;
    uint8 times = 20;

    adc_average_value = GetAdcAverageValue(adc_cfg->ADCx, adc_cfg->adc_channel, times);

   *(uint16 *)read_param->buffer = adc_average_value;
   read_param->read_length = 2;

   return read_param->read_length;
}

static uint32 AdcDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    uint8 adc_channel;

    struct AdcDriver *adc_drv = (struct AdcDriver *)drv;
    struct AdcHardwareDevice *adc_dev = (struct AdcHardwareDevice *)adc_drv->driver.owner_bus->owner_haldev;
    struct HwAdc *adc_cfg = (struct HwAdc *)adc_dev->haldev.private_data;

    switch (configure_info->configure_cmd)
    {
        case OPE_CFG:
            adc_cfg->adc_channel = *(uint8 *)configure_info->private_data;
            if (adc_cfg->adc_channel > 18) {
                KPrintf("AdcDrvConfigure set adc channel(0-18) %u error!", adc_cfg->adc_channel);
                adc_cfg->adc_channel = 0;
                ret = ERROR;
            }
            break;
        default:
            break;
    }

    return ret;
}

static const struct AdcDevDone dev_done =
{
    AdcOpen,
    AdcClose,
    NONE,
    AdcRead,
};

int HwAdcInit(void)
{
    x_err_t ret = EOK;

#ifdef BSP_USING_ADC1
    static struct AdcBus adc1_bus;
    static struct AdcDriver adc1_drv;
    static struct AdcHardwareDevice adc1_dev;
    static struct HwAdc adc1_cfg;

    adc1_drv.configure = AdcDrvConfigure;

    ret = AdcBusInit(&adc1_bus, ADC1_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC1 bus init error %d\n", ret);
        return ERROR;
    }

    ret = AdcDriverInit(&adc1_drv, ADC1_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("ADC1 driver init error %d\n", ret);
        return ERROR;
    }
    ret = AdcDriverAttachToBus(ADC1_DRIVER_NAME, ADC1_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC1 driver attach error %d\n", ret);
        return ERROR;
    }

    adc1_dev.adc_dev_done = &dev_done;
    adc1_cfg.ADCx = CM_ADC1;
    adc1_cfg.adc_channel = 0;

    ret = AdcDeviceRegister(&adc1_dev, (void *)&adc1_cfg, ADC1_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("ADC1 device register error %d\n", ret);
        return ERROR;
    }
    ret = AdcDeviceAttachToBus(ADC1_DEVICE_NAME, ADC1_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC1 device register error %d\n", ret);
        return ERROR;
    }
#endif 

#ifdef BSP_USING_ADC2
    static struct AdcBus adc2_bus;
    static struct AdcDriver adc2_drv;
    static struct AdcHardwareDevice adc2_dev;
    static struct HwAdc adc2_cfg;

    adc2_drv.configure = AdcDrvConfigure;

    ret = AdcBusInit(&adc2_bus, ADC2_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC2 bus init error %d\n", ret);
        return ERROR;
    }

    ret = AdcDriverInit(&adc2_drv, ADC2_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("ADC2 driver init error %d\n", ret);
        return ERROR;
    }
    ret = AdcDriverAttachToBus(ADC2_DRIVER_NAME, ADC2_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC2 driver attach error %d\n", ret);
        return ERROR;
    }

    adc2_dev.adc_dev_done = &dev_done;
    adc2_cfg.ADCx = CM_ADC2;
    adc2_cfg.adc_channel = 0;

    ret = AdcDeviceRegister(&adc2_dev, (void *)&adc2_cfg, ADC2_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("ADC2 device register error %d\n", ret);
        return ERROR;
    }
    ret = AdcDeviceAttachToBus(ADC2_DEVICE_NAME, ADC2_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC2 device register error %d\n", ret);
        return ERROR;
    }
#endif 

#ifdef BSP_USING_ADC3
    static struct AdcBus adc3_bus;
    static struct AdcDriver adc3_drv;
    static struct AdcHardwareDevice adc3_dev;
    static struct HwAdc adc3_cfg;

    adc3_drv.configure = AdcDrvConfigure;

    ret = AdcBusInit(&adc3_bus, ADC3_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC3 bus init error %d\n", ret);
        return ERROR;
    }

    ret = AdcDriverInit(&adc3_drv, ADC3_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("ADC3 driver init error %d\n", ret);
        return ERROR;
    }
    ret = AdcDriverAttachToBus(ADC3_DRIVER_NAME, ADC3_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC3 driver attach error %d\n", ret);
        return ERROR;
    }

    adc3_dev.adc_dev_done = &dev_done;
    adc3_cfg.ADCx = CM_ADC3;
    adc3_cfg.adc_channel = 0;

    ret = AdcDeviceRegister(&adc3_dev, (void *)&adc3_cfg, ADC3_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("ADC3 device register error %d\n", ret);
        return ERROR;
    }
    ret = AdcDeviceAttachToBus(ADC3_DEVICE_NAME, ADC3_BUS_NAME);
    if (ret != EOK) {
        KPrintf("ADC3 device register error %d\n", ret);
        return ERROR;
    }
#endif 

    return ret;

}





