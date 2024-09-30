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
* @file connect_usb.c
* @brief support edu-arm32-board usb function and register to bus framework
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-11-07
*/

/*************************************************
File name: connect_usb.c
Description: support edu-arm32-board usb function and register to bus framework
Others: 
History: 
1. Date: 2022-11-07
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board usb configure, write and read
2. support edu-arm32-board usb bus device and driver register
*************************************************/

#include <connect_usb.h>

usb_core_instance usb_app_instance;
USBH_HOST usb_app_host;

static void UsbHostTask(void *parameter);

#if defined(FS_VFS)
void UsbMountFileSystem()
{
    if (MountFilesystem(USB_BUS_NAME, USB_DEVICE_NAME, USB_DRIVER_NAME, FSTYPE_FATFS, UDISK_MOUNTPOINT) == 0)
        KPrintf("Mount FAT on Udisk successful.\n");
    else
        KPrintf("Mount FAT on Udisk failed.\n");
}

void UsbUnmountFileSystem()
{
    UnmountFileSystem(UDISK_MOUNTPOINT);
}

#ifdef MOUNT_USB
int MountUsb(void)
{
    int32 usb_host_task = 0;
    usb_host_task = KTaskCreate("usbh", UsbHostTask, NONE,
                           USB_HOST_STACK_SIZE, 8);
    if(usb_host_task < 0) {		
		KPrintf("usb_host_task create failed ...%s %d.\n", __FUNCTION__, __LINE__);
		return ERROR;
	}

    StartupKTask(usb_host_task);

    return 0;
}
#endif
#endif

static uint32 UsbHostOpen(void *dev)
{
    return EOK;
}

static uint32 UsbHostClose(void *dev)
{
    return EOK;
}

static uint32 UsbHostRead(void *dev, struct BusBlockReadParam *read_param)
{
    USB_HOST_MSC_STATUS status = USB_HOST_MSC_OK;

    if (host_driver_ifdevconnected(&usb_app_instance) != 0UL) {

        do {
            status = usb_host_msc_Read10(&usb_app_instance, (uint8 *)read_param->buffer, read_param->pos, USB_SINGLE_BLOCK_SIZE * (uint32_t)read_param->size);
            usb_host_msc_botxferprocess(&usb_app_instance, &usb_app_host);

            if (host_driver_ifdevconnected(&usb_app_instance) == 0UL) {
                return 0;
            }
        } while (USB_HOST_MSC_BUSY == status);
    }

    if (USB_HOST_MSC_OK == status) {
        return read_param->size;
    }

    return 0;
}

static uint32 UsbHostWrite(void *dev, struct BusBlockWriteParam *write_param)
{
    USB_HOST_MSC_STATUS status = USB_HOST_MSC_OK;

    if (host_driver_ifdevconnected(&usb_app_instance) != 0UL) {
        do {
            status = usb_host_msc_Write10(&usb_app_instance, (uint8 *)write_param->buffer, write_param->pos, USB_SINGLE_BLOCK_SIZE * (uint32_t)write_param->size);
            usb_host_msc_botxferprocess(&usb_app_instance, &usb_app_host);

            if (host_driver_ifdevconnected(&usb_app_instance) == 0UL) {
                return 0;
            }
        } while (USB_HOST_MSC_BUSY == status);
    }

    if (USB_HOST_MSC_OK == status) {
        return write_param->size;
    }

    return 0;
}

static int UsbControl(struct HardwareDev *dev, struct HalDevBlockParam *block_param)
{
    NULL_PARAM_CHECK(dev);

    if (OPER_BLK_GETGEOME == block_param->cmd) {
        block_param->dev_block.size_perbank = USB_SINGLE_BLOCK_SIZE;
        block_param->dev_block.block_size = USB_SINGLE_BLOCK_SIZE;
        block_param->dev_block.bank_num = USB_HOST_MSC_Param.MSC_Capacity;
    }

    return EOK;
}

/*manage the usb device operations*/
static const struct UsbDevDone dev_done =
{
    .open = UsbHostOpen,
    .close = UsbHostClose,
    .write = UsbHostWrite,
    .read = UsbHostRead,
};

static void UsbHostTask(void *parameter)
{
    usb_host_init(&usb_app_instance, &usb_app_host, &USBH_MSC_cb, &USR_cb);
    while (1) {
        usb_host_mainprocess(&usb_app_instance, &usb_app_host);
    }
}

/*Init usb host bus、driver*/
static int BoardUsbBusInit(struct UsbBus *usb_bus, struct UsbDriver *usb_driver)
{
    x_err_t ret = EOK;

    /*Init the usb bus */
    ret = UsbBusInit(usb_bus, USB_BUS_NAME);
    if (EOK != ret) {
        KPrintf("board_usb_init UsbBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the usb driver*/
    ret = UsbDriverInit(usb_driver, USB_DRIVER_NAME);
    if (EOK != ret){
        KPrintf("board_usb_init UsbDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the usb driver to the usb bus*/
    ret = UsbDriverAttachToBus(USB_DRIVER_NAME, USB_BUS_NAME);
    if (EOK != ret) {
        KPrintf("board_usb_init USEDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

/*Attach the usb device to the usb bus*/
static int BoardUsbDevBend(void)
{
    x_err_t ret = EOK;
    static struct UsbHardwareDevice usb_device;
    memset(&usb_device, 0, sizeof(struct UsbHardwareDevice));

    usb_device.dev_done = &dev_done;
    usb_device.haldev.dev_block_control = UsbControl;

    ret = USBDeviceRegister(&usb_device, NONE, USB_DEVICE_NAME);
    if (EOK != ret) {
        KPrintf("USBDeviceRegister device %s error %d\n", USB_DEVICE_NAME, ret);
        return ERROR;
    }

    ret = USBDeviceAttachToBus(USB_DEVICE_NAME, USB_BUS_NAME);
    if (EOK != ret) {
        KPrintf("USBDeviceAttachToBus device %s error %d\n", USB_DEVICE_NAME, ret);
        return ERROR;
    }

    return ret;
}

int HwUsbHostInit(void)
{
    x_err_t ret = EOK;

    static struct UsbBus usb_bus;
    memset(&usb_bus, 0, sizeof(struct UsbBus));

    static struct UsbDriver usb_driver;
    memset(&usb_driver, 0, sizeof(struct UsbDriver));

    ret = BoardUsbBusInit(&usb_bus, &usb_driver);
    if (EOK != ret) {
        KPrintf("BoardUsbBusInit error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardUsbDevBend();
    if (EOK != ret) {
        KPrintf("BoardUsbDevBend error ret %u\n", ret);
        return ERROR;
    }

    return ret;
}

