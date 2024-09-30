/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_host_user.c
 * @brief user application layer.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/**
* @file usb_host_user.c
* @brief support edu-arm32-board usb function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-11-07
*/

/*************************************************
File name: usb_host_user.c
Description: support edu-arm32-board usb function
Others: 
History: 
1. Date: 2022-11-07
Author: AIIT XUOS Lab
Modification: 
1. delete useless usb host configure and define
2. add KPrintf function
3. add UsbMountFileSystem() and UsbUnmountFileSystem()
*************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <device.h>
#include "usb_host_user.h"
#include "usb_host_msc_class.h"
#include "usb_host_msc_scsi.h"
#include "usb_host_msc_bot.h"
#include "usb_host_driver.h"

/**
 * @addtogroup HC32F4A0_DDL_Applications
 * @{
 */

/**
 * @addtogroup USB_Host_Msc
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
extern void UsbMountFileSystem();
extern void UsbUnmountFileSystem();

/* USBH_USR_Private_Macros */
extern usb_core_instance usb_app_instance;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
usb_host_user_callback_func USR_cb = {
    &host_user_init,
    &host_user_denint,
    &host_user_devattached,
    &host_user_devreset,
    &host_user_devdisconn,
    &host_user_overcurrent,
    &host_user_devspddetected,
    &host_user_devdescavailable,
    &host_user_devaddrdistributed,
    &host_user_cfgdescavailable,
    &host_user_mfcstring,
    &host_user_productstring,
    &host_user_serialnum,
    &host_user_enumcompl,
    &host_user_userinput,
    &host_user_msc_app,
    &host_user_devunsupported,
    &host_user_unrecoverederror

};

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static int usb_connect_status = 0;

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint8_t USB_HOST_USER_AppState = USH_USR_FS_INIT;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Displays the message on terminal for host lib initialization
 * @param  None
 * @retval None
 */
void host_user_init(void)
{
    static uint8_t startup = 0U;

    if (startup == 0U) {
        startup = 1U;
        //KPrintf("USB Host library v2.1.0 started\r\n");
    }
}

/**
 * @brief  Displays the message on terminal via DDL_Printf
 * @param  None
 * @retval None
 */
void host_user_devattached(void)
{
    KPrintf("USB device attached\r\n");
}

/**
 * @brief  host_user_unrecoverederror
 * @param  None
 * @retval None
 */
void host_user_unrecoverederror(void)
{
    KPrintf("USB device unrecovered error\r\n");
}

/**
 * @brief  Device disconnect event
 * @param  None
 * @retval None
 */
void host_user_devdisconn(void)
{
    KPrintf("USB device disconnect\r\n");
    usb_connect_status = 0;
    UsbUnmountFileSystem();
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 * @param  None
 * @retval None
 */
void host_user_devreset(void)
{
    /* callback for USB-Reset */
    KPrintf("USB device reset\r\n");
}

/**
 * @brief  host_user_devspddetected
 * @param  [in] DeviceSpeed      USB speed
 * @retval None
 */
void host_user_devspddetected(uint8_t DeviceSpeed)
{
    if (DeviceSpeed == PRTSPD_FULL_SPEED) {
        KPrintf("USB device speed PRTSPD_FULL_SPEED\r\n");
    } else if (DeviceSpeed == PRTSPD_LOW_SPEED) {
        KPrintf("USB device speed PRTSPD_LOW_SPEED\r\n");
    } else {
        KPrintf("USB device speed error\r\n");
    }
}

/**
 * @brief  host_user_devdescavailable
 * @param  [in] DeviceDesc       device descriptor
 * @retval None
 */
void host_user_devdescavailable(void *DeviceDesc)
{
    usb_host_devdesc_typedef *hs;
    hs = DeviceDesc;
    KPrintf("USB device VID : %04lXh PID : %04lXh\r\n", (uint32_t)(*hs).idVendor, (uint32_t)(*hs).idProduct);
}

/**
 * @brief  host_user_devaddrdistributed
 * @param  None
 * @retval None
 */
void host_user_devaddrdistributed(void)
{
}

/**
 * @brief  host_user_cfgdescavailable
 * @param  [in] cfgDesc          Configuration desctriptor
 * @param  [in] itfDesc          Interface desctriptor
 * @param  [in] epDesc           Endpoint desctriptor
 * @retval None
 */
void host_user_cfgdescavailable(usb_host_cfgdesc_typedef *cfgDesc,
                                usb_host_itfdesc_typedef *itfDesc,
                                USB_HOST_EPDesc_TypeDef *epDesc)
{
    usb_host_itfdesc_typedef *id;

    id = itfDesc;
    if ((*id).bInterfaceClass  == 0x08U) {
        KPrintf("USB Mass storage device connected\r\n");
    } else if ((*id).bInterfaceClass  == 0x03U) {
        KPrintf("USB HID device connected\r\n");
    } else {
        ;
    }
}

/**
 * @brief  Displays the message on terminal for Manufacturer String
 * @param  [in] ManufacturerString
 * @retval None
 */
void host_user_mfcstring(void *ManufacturerString)
{
    KPrintf("Manufacturer : %s\r\n", (char *)ManufacturerString);
}

/**
 * @brief  Displays the message on terminal for product String
 * @param  [in] ProductString
 * @retval None
 */
void host_user_productstring(void *ProductString)
{
    KPrintf("Product : %s\r\n", (char *)ProductString);
}

/**
 * @brief  Displays the message on terminal for SerialNum_String
 * @param  [in] SerialNumString
 * @retval None
 */
void host_user_serialnum(void *SerialNumString)
{
    KPrintf("Serial Number : %s\r\n", (char *)SerialNumString);
}

/**
 * @brief  User response request is displayed to ask application jump to class
 * @param  None
 * @retval None
 */
void host_user_enumcompl(void)
{
}

/**
 * @brief  Device is not supported
 * @param  None
 * @retval None
 */
void host_user_devunsupported(void)
{
    KPrintf("USB Device not supported.\r\n");
}

/**
 * @brief  User Action for application state entry
 * @param  None
 * @retval HOST_USER_STATUS     User response for key button
 */
HOST_USER_STATUS host_user_userinput(void)
{
}

/**
 * @brief  Over Current Detected on VBUS
 * @param  None
 * @retval None
 */
void host_user_overcurrent(void)
{
    KPrintf("USB HOST Overcurrent detected.\r\n");
}

/**
 * @brief  Demo application for mass storage
 * @param  None
 * @retval None
 */
int host_user_msc_app(void)
{
    if (0 == usb_connect_status) {
        KPrintf("ready to mount file system\n");
        UsbMountFileSystem();
        usb_connect_status = 1;
    }
    
    return ((int)0);
}

/**
 * @brief  Deint User state and associated variables
 * @param  None
 * @retval None
 */
void host_user_denint(void)
{
    USB_HOST_USER_AppState = USH_USR_FS_INIT;
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
