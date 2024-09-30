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
* @file bus_kpu.c
* @brief register kpu bus function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-12-19
*/

#include <bus_kpu.h>
#include <dev_kpu.h>

/*Register the KPU BUS*/
int KpuBusInit(struct KpuBus *kpu_bus, const char *bus_name)
{
    NULL_PARAM_CHECK(kpu_bus);
    NULL_PARAM_CHECK(bus_name);

    x_err_t ret = EOK;

    if (BUS_INSTALL != kpu_bus->bus.bus_state) {
        strncpy(kpu_bus->bus.bus_name, bus_name, NAME_NUM_MAX);

        kpu_bus->bus.bus_type = TYPE_KPU_BUS;
        kpu_bus->bus.bus_state = BUS_INSTALL;
        kpu_bus->bus.private_data = kpu_bus->private_data;

        ret = BusRegister(&kpu_bus->bus);
        if (EOK != ret) {
            KPrintf("KpuBusInit BusRegister error %u\n", ret);
            return ret;
        }
    } else {
        KPrintf("KpuBusInit BusRegister bus has been register state%u\n", kpu_bus->bus.bus_state);        
    }

    return ret;
}

/*Register the KPU Driver*/
int KpuDriverInit(struct KpuDriver *kpu_driver, const char *driver_name)
{
    NULL_PARAM_CHECK(kpu_driver);
    NULL_PARAM_CHECK(driver_name);

    x_err_t ret = EOK;

    if (DRV_INSTALL != kpu_driver->driver.driver_state) {
        kpu_driver->driver.driver_type = TYPE_KPU_DRV;
        kpu_driver->driver.driver_state = DRV_INSTALL;

        strncpy(kpu_driver->driver.drv_name, driver_name, NAME_NUM_MAX);

        kpu_driver->driver.private_data = kpu_driver->private_data;

        kpu_driver->driver.configure = kpu_driver->configure;

        ret = KpuDriverRegister(&kpu_driver->driver);
        if (EOK != ret) {
            KPrintf("KpuDriverInit DriverRegister error %u\n", ret);
            return ret;
        }
    } else {
        KPrintf("KpuDriverInit DriverRegister driver has been register state%u\n", kpu_driver->driver.driver_state);
    }

    return ret;
}

/*Release the KPU device*/
int KpuReleaseBus(struct KpuBus *kpu_bus)
{
    NULL_PARAM_CHECK(kpu_bus);

    return BusRelease(&kpu_bus->bus);
}

/*Register the KPU Driver to the KPU BUS*/
int KpuDriverAttachToBus(const char *drv_name, const char *bus_name)
{
    NULL_PARAM_CHECK(drv_name);
    NULL_PARAM_CHECK(bus_name);
    
    x_err_t ret = EOK;

    struct Bus *bus;
    struct Driver *driver;

    bus = BusFind(bus_name);
    if (NONE == bus) {
        KPrintf("KpuDriverAttachToBus find kpu bus error!name %s\n", bus_name);
        return ERROR;
    }

    if (TYPE_KPU_BUS == bus->bus_type) {
        driver = KpuDriverFind(drv_name, TYPE_KPU_DRV);
        if (NONE == driver) {
            KPrintf("KpuDriverAttachToBus find kpu driver error!name %s\n", drv_name);
            return ERROR;
        }

        if (TYPE_KPU_DRV == driver->driver_type) {
            ret = DriverRegisterToBus(bus, driver);
            if (EOK != ret) {
                KPrintf("KpuDriverAttachToBus DriverRegisterToBus error %u\n", ret);
                return ERROR;
            }
        }
    }

    return ret;
}
