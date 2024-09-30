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
* @file bus_camera.h
* @brief define camera bus and drv function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-11-15
*/

#ifndef BUS_CAMERA_H
#define BUS_CAMERA_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif


struct CameraDriver
{
    struct Driver driver;

    uint32 (*configure) (void *drv, struct BusConfigureInfo *configure_info);

    void *private_data;
};

struct CameraBus
{
    struct Bus bus;

    void *private_data;
};

/*Register the CAMERA bus*/
int CameraBusInit(struct CameraBus *camera_bus, const char *bus_name);

/*Register the CAMERA driver*/
int CameraDriverInit(struct CameraDriver *camera_driver, const char *driver_name);

/*Release the CAMERA device*/
int CameraReleaseBus(struct CameraBus *camera_bus);

/*Register the CAMERA driver to the CAMERA bus*/
int CameraDriverAttachToBus(const char *drv_name, const char *bus_name);

/*Register the driver, manage with the double linklist*/
int CameraDriverRegister(struct Driver *driver);

/*Find the register driver*/
DriverType CameraDriverFind(const char *drv_name, enum DriverType_e drv_type);

#ifdef __cplusplus
}
#endif

#endif