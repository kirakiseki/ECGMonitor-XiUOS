/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-13     bernard      first version
 * 2012-05-15     lgnq         modified according bernard's implementation.
 * 2012-05-28     bernard      code cleanup
 * 2012-11-23     bernard      fix compiler warning.
 * 2013-02-20     bernard      use RT_CAMERA_RB_BUFSZ to define
 *                             the size of ring buffer.
 * 2014-07-10     bernard      rewrite camera framework
 * 2014-12-31     bernard      use open_flag for poll_tx stream mode.
 * 2015-05-19     Quintin      fix DMA tx mod tx_dma->activated flag !=RT_FALSE BUG
 *                             in open function.
 * 2015-11-10     bernard      fix the poll rx issue when there is no data.
 * 2016-05-10     armink       add fifo mode to DMA rx when camera->config.bufsz != 0.
 * 2017-01-19     aubr.cool    prevent change camera rx bufsz when camera is opened.
 * 2017-11-07     JasonJia     fix data bits error issue when using tcsetattr.
 * 2017-11-15     JasonJia     fix poll rx issue when data is full.
 *                             add TCFLSH and FIONREAD support.
 * 2018-12-08     Ernest Chen  add DMA choice
 * 2020-09-14     WillianChan  add a line feed to the carriage return character
 *                             when using interrupt tx
 */

/**
* @file dev_camera.c
* @brief register camera dev function using bus driver framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-24
*/

/*************************************************
File name: dev_camera.c
Description: support camera dev INT and DMA configure、transfer data
Others: take RT-Thread v4.0.2/components/driver/camera/camera.c for references
                https://github.com/RT-Thread/rt-thread/tree/v4.0.2
History: 
1. Date: 2021-04-24
Author: AIIT XUOS Lab
Modification: 
1. support camera dev register, configure, write and read
2. add bus driver framework support, include INT and DMA mode
*************************************************/

#include <bus_camera.h>
#include <dev_camera.h>

static DoubleLinklistType cameradev_linklist;

static uint32 CameraDevOpen(void *dev)
{
    NULL_PARAM_CHECK(dev);

    x_err_t ret = EOK;
    struct CameraHardwareDevice *camera_dev = (struct CameraHardwareDevice *)dev;

    ret = camera_dev->camera_dev_done->dev_open(dev);

    return EOK;
}

static uint32 CameraDevClose(void *dev)
{
    NULL_PARAM_CHECK(dev);

    x_err_t ret = EOK;
    struct CameraHardwareDevice *camera_dev = (struct CameraHardwareDevice *)dev;

    ret = camera_dev->camera_dev_done->dev_close(dev);
    return EOK;
}

static uint32 CameraDevWrite(void *dev, struct BusBlockWriteParam *write_param)
{
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(write_param);

    x_err_t ret = EOK;

    struct CameraHardwareDevice *camera_dev = (struct CameraHardwareDevice *)dev;
    ret = camera_dev->camera_dev_done->dev_write(dev,write_param);

    return ret;
}

static uint32 CameraDevRead(void *dev, struct BusBlockReadParam *read_param)
{
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(read_param);

    x_err_t ret = EOK;

    struct CameraHardwareDevice *camera_dev = (struct CameraHardwareDevice *)dev;
    ret = camera_dev->camera_dev_done->dev_read(dev,read_param);

    return EOK;
}


static const struct HalDevDone dev_done =
{
    .open = CameraDevOpen,
    .close = CameraDevClose,
    .write = CameraDevWrite,
    .read = CameraDevRead,
};

/*Create the camera device linklist*/
static void CameraDeviceLinkInit()
{
    InitDoubleLinkList(&cameradev_linklist);
}

HardwareDevType CameraDeviceFind(const char *dev_name, enum DevType dev_type)
{
    NULL_PARAM_CHECK(dev_name);
    
    struct HardwareDev *device = NONE;

    DoubleLinklistType *node = NONE;
    DoubleLinklistType *head = &cameradev_linklist;

    for (node = head->node_next; node != head; node = node->node_next) {
        device = SYS_DOUBLE_LINKLIST_ENTRY(node, struct HardwareDev, dev_link);
        if ((!strcmp(device->dev_name, dev_name)) && (dev_type == device->dev_type)) {
            return device;
        }
    }

    KPrintf("CameraDeviceFind cannot find the %s device.return NULL\n", dev_name);
    return NONE;
}

int CameraDeviceRegister(struct CameraHardwareDevice *camera_device, void *camera_param, const char *device_name)
{
    NULL_PARAM_CHECK(camera_device);
    NULL_PARAM_CHECK(device_name);

    x_err_t ret = EOK;    
    static x_bool dev_link_flag = RET_FALSE;

    if (!dev_link_flag) {
        CameraDeviceLinkInit();
        dev_link_flag = RET_TRUE;
    }

    if (DEV_INSTALL != camera_device->haldev.dev_state) {
        strncpy(camera_device->haldev.dev_name, device_name, NAME_NUM_MAX);
        camera_device->haldev.dev_type = TYPE_CAMERA_DEV;
        camera_device->haldev.dev_state = DEV_INSTALL;
        camera_device->haldev.dev_done = (struct HalDevDone *)&dev_done;


        DoubleLinkListInsertNodeAfter(&cameradev_linklist, &(camera_device->haldev.dev_link));
    } else {
        KPrintf("CameraDeviceRegister device has been register state%u\n", camera_device->haldev.dev_state);        
    }

    return ret;
}

int CameraDeviceAttachToBus(const char *dev_name, const char *bus_name)
{
    NULL_PARAM_CHECK(dev_name);
    NULL_PARAM_CHECK(bus_name);
    
    x_err_t ret = EOK;

    struct Bus *bus;
    struct HardwareDev *device;

    bus = BusFind(bus_name);
    if (NONE == bus) {
        KPrintf("CameraDeviceAttachToBus find camera bus error!name %s\n", bus_name);
        return ERROR;
    }
    
    if (TYPE_CAMERA_BUS == bus->bus_type) {
        device = CameraDeviceFind(dev_name, TYPE_CAMERA_DEV);
        if (NONE == device) {
            KPrintf("CameraDeviceAttachToBus find camera device error!name %s\n", dev_name);
            return ERROR;
        }

        if (TYPE_CAMERA_DEV == device->dev_type) {
            ret = DeviceRegisterToBus(bus, device);
            if (EOK != ret) {
                KPrintf("CameraDeviceAttachToBus DeviceRegisterToBus error %u\n", ret);
                return ERROR;
            }
        }
    }

    return EOK;
}
