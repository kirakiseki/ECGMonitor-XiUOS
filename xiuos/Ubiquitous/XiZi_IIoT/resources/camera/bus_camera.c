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
* @file bus_camera.c
* @brief register camera bus function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-24
*/

#include <bus_camera.h>
#include <dev_camera.h>

/*Register the CAMERA BUS*/
int CameraBusInit(struct CameraBus *camera_bus, const char *bus_name)
{
    NULL_PARAM_CHECK(camera_bus);
    NULL_PARAM_CHECK(bus_name);

    x_err_t ret = EOK;

    if (BUS_INSTALL != camera_bus->bus.bus_state) {
        strncpy(camera_bus->bus.bus_name, bus_name, NAME_NUM_MAX);

        camera_bus->bus.bus_type = TYPE_CAMERA_BUS;
        camera_bus->bus.bus_state = BUS_INSTALL;
        camera_bus->bus.private_data = camera_bus->private_data;

        ret = BusRegister(&camera_bus->bus);
        if (EOK != ret) {
            KPrintf("CameraBusInit BusRegister error %u\n", ret);
            return ret;
        }
    } else {
        KPrintf("CameraBusInit BusRegister bus has been register state%u\n", camera_bus->bus.bus_state);        
    }

    return ret;
}

/*Register the CAMERA Driver*/
int CameraDriverInit(struct CameraDriver *camera_driver, const char *driver_name)
{
    NULL_PARAM_CHECK(camera_driver);
    NULL_PARAM_CHECK(driver_name);

    x_err_t ret = EOK;

    if (DRV_INSTALL != camera_driver->driver.driver_state) {
        camera_driver->driver.driver_type = TYPE_CAMERA_DRV;
        camera_driver->driver.driver_state = DRV_INSTALL;

        strncpy(camera_driver->driver.drv_name, driver_name, NAME_NUM_MAX);

        camera_driver->driver.private_data = camera_driver->private_data;

        camera_driver->driver.configure = camera_driver->configure;

        ret = CameraDriverRegister(&camera_driver->driver);
        if (EOK != ret) {
            KPrintf("CameraDriverInit DriverRegister error %u\n", ret);
            return ret;
        }
    } else {
        KPrintf("CameraDriverInit DriverRegister driver has been register state%u\n", camera_driver->driver.driver_state);
    }

    return ret;
}

/*Release the CAMERA device*/
int CameraReleaseBus(struct CameraBus *camera_bus)
{
    NULL_PARAM_CHECK(camera_bus);

    return BusRelease(&camera_bus->bus);
}

/*Register the CAMERA Driver to the CAMERA BUS*/
int CameraDriverAttachToBus(const char *drv_name, const char *bus_name)
{
    NULL_PARAM_CHECK(drv_name);
    NULL_PARAM_CHECK(bus_name);
    
    x_err_t ret = EOK;

    struct Bus *bus;
    struct Driver *driver;

    bus = BusFind(bus_name);
    if (NONE == bus) {
        KPrintf("CameraDriverAttachToBus find camera bus error!name %s\n", bus_name);
        return ERROR;
    }

    if (TYPE_CAMERA_BUS == bus->bus_type) {
        driver = CameraDriverFind(drv_name, TYPE_CAMERA_DRV);
        if (NONE == driver) {
            KPrintf("CameraDriverAttachToBus find camera driver error!name %s\n", drv_name);
            return ERROR;
        }

        if (TYPE_CAMERA_DRV == driver->driver_type) {
            ret = DriverRegisterToBus(bus, driver);
            if (EOK != ret) {
                KPrintf("CameraDriverAttachToBus DriverRegisterToBus error %u\n", ret);
                return ERROR;
            }
        }
    }

    return ret;
}
