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
* @file connect_usb.h
* @brief define edu-arm32-board usb function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2022-11-07
*/

#ifndef CONNECT_USB_H
#define CONNECT_USB_H

#include <device.h>
#include <hc32_ll_utility.h>
#include <hc32_ll_gpio.h>
#include <usb_lib.h>
#include <usb_host_user.h>
#include <usb_host_driver.h>
#include <usb_host_core.h>
#include <usb_host_msc_class.h>
#include <usb_host_msc_scsi.h>
#include <usb_host_msc_bot.h>
#include <usb_host_int.h>

#if defined(FS_VFS)
#include <iot-vfs.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define USB_HOST_STACK_SIZE 4096

#define USB_SINGLE_BLOCK_SIZE 512

int HwUsbHostInit(void);

#ifdef __cplusplus
}
#endif

#endif
