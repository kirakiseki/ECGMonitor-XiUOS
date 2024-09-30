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
* @file bus_kpu.h
* @brief define kpu bus and drv function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-12-19
*/

#ifndef BUS_KPU_H
#define BUS_KPU_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif


struct KpuDriver
{
    struct Driver driver;

    uint32 (*configure) (void *drv, struct BusConfigureInfo *configure_info);

    void *private_data;
};

struct KpuBus
{
    struct Bus bus;

    void *private_data;
};

/*Register the KPU bus*/
int KpuBusInit(struct KpuBus *kpu_bus, const char *bus_name);

/*Register the KPU driver*/
int KpuDriverInit(struct KpuDriver *kpu_driver, const char *driver_name);

/*Release the KPU device*/
int KpuReleaseBus(struct KpuBus *kpu_bus);

/*Register the KPU driver to the KPU bus*/
int KpuDriverAttachToBus(const char *drv_name, const char *bus_name);

/*Register the driver, manage with the double linklist*/
int KpuDriverRegister(struct Driver *driver);

/*Find the register driver*/
DriverType KpuDriverFind(const char *drv_name, enum DriverType_e drv_type);

#ifdef __cplusplus
}
#endif

#endif