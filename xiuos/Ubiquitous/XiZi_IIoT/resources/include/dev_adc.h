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
* @file dev_adc.h
* @brief define adc dev function using bus driver framework
* @version 1.1
* @author AIIT XUOS Lab
* @date 2021-12-28
*/

#ifndef DEV_ADC_H
#define DEV_ADC_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif

struct AdcHardwareDevice;

struct AdcDevDone
{
    uint32 (*open) (void *dev);
    uint32 (*close) (void *dev);
    uint32 (*write) (void *dev,struct BusBlockWriteParam *write_param);
    uint32 (*read) (void *dev, struct BusBlockReadParam *read_param);
};

struct AdcHardwareDevice
{
    struct HardwareDev haldev;
    const struct AdcDevDone *adc_dev_done;

    void *private_data;
};

/*Register the ADC device*/
int AdcDeviceRegister(struct AdcHardwareDevice *adc_device, void *adc_param, const char *device_name);

/*Register the ADC device to the ADC bus*/
int AdcDeviceAttachToBus(const char *dev_name, const char *bus_name);

/*Find the register ADC device*/
HardwareDevType AdcDeviceFind(const char *dev_name, enum DevType dev_type);

#ifdef __cplusplus
}
#endif

#endif
