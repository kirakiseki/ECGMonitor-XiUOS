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
* @file dev_camera.h
* @brief define camera dev function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-11-15
*/

#ifndef DEV_CAMERA_H
#define DEV_CAMERA_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif


struct CameraDataStandard
{
    uint16 addr;
    uint16 flags;
    uint16 len;
    uint16 retries;
    uint8 *buf;

    struct CameraDataStandard *next;
};

struct CameraDevDone
{
    uint32 (*dev_open) (void *camera_device);
    uint32 (*dev_close) (void *camera_device);
    uint32 (*dev_write) (void *camera_device, struct BusBlockWriteParam *msg);
    uint32 (*dev_read) (void *camera_device, struct BusBlockReadParam *msg);
};

struct CameraHardwareDevice
{
    struct HardwareDev haldev;
    const struct CameraDevDone *camera_dev_done;
};

/*Register the CAMERA device*/
int CameraDeviceRegister(struct CameraHardwareDevice *camera_device, void *camera_param, const char *device_name);

/*Register the CAMERA device to the CAMERA bus*/
int CameraDeviceAttachToBus(const char *dev_name, const char *bus_name);

/*Find the register CAMERA device*/
HardwareDevType CameraDeviceFind(const char *dev_name, enum DevType dev_type);

#ifdef __cplusplus
}
#endif

#endif
