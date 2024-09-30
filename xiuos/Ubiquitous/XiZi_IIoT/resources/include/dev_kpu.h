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
* @file dev_kpu.h
* @brief define kpu dev function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-12-19
*/

#ifndef DEV_KPU_H
#define DEV_KPU_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif


struct KpuDataStandard
{
    uint16 addr;
    uint16 flags;
    uint16 len;
    uint16 retries;
    uint8 *buf;

    struct KpuDataStandard *next;
};

struct KpuDevDone
{
    uint32 (*dev_open) (void *kpu_device);
    uint32 (*dev_close) (void *kpu_device);
    uint32 (*dev_write) (void *kpu_device, struct BusBlockWriteParam *msg);
    uint32 (*dev_read) (void *kpu_device, struct BusBlockReadParam *msg);
};

struct KpuHardwareDevice
{
    struct HardwareDev haldev;
    const struct KpuDevDone *kpu_dev_done;
};

/*Register the KPU device*/
int KpuDeviceRegister(struct KpuHardwareDevice *kpu_device, void *kpu_param, const char *device_name);

/*Register the KPU device to the KPU bus*/
int KpuDeviceAttachToBus(const char *dev_name, const char *bus_name);

/*Find the register KPU device*/
HardwareDevType KpuDeviceFind(const char *dev_name, enum DevType dev_type);

#ifdef __cplusplus
}
#endif

#endif
