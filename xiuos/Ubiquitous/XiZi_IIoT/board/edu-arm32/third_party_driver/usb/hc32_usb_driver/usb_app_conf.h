/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_app_conf.h
 * @brief low level driver configuration
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
#ifndef __USB_APP_CONF_H__
#define __USB_APP_CONF_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/

/* USB MODE CONFIGURATION */
/*
USB_FS_MODE, USB_HS_MODE, USB_HS_EXTERNAL_PHY defined comment
(1) If only defined USB_FS_MODE:
    MCU USBFS core work in full speed using internal PHY.
(2) If only defined USB_HS_MODE:
    MCU USBHS core work in full speed using internal PHY.
(3) If both defined USB_HS_MODE && USB_HS_EXTERNAL_PHY
    MCU USBHS core work in high speed using external PHY.
(4) Other combination:
    Not support, forbid!!
*/

//#define USB_HS_MODE
#define USB_FS_MODE

#define USE_HOST_MODE

#ifdef USB_HS_MODE
#define USB_HS_EXTERNAL_PHY
#endif

#ifndef USB_HS_MODE
#ifndef USB_FS_MODE
#error  "USB_HS_MODE or USB_FS_MODE should be defined"
#endif
#endif

#ifndef USE_DEVICE_MODE
#ifndef USE_HOST_MODE
#error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
#endif
#endif

/* USB FIFO CONFIGURATION */
#ifdef USB_FS_MODE
#define RX_FIFO_FS_SIZE                          (128U)
#define TXH_NP_FS_FIFOSIZ                        (32U)
#define TXH_P_FS_FIFOSIZ                         (64U)

#if ((RX_FIFO_FS_SIZE + TXH_NP_FS_FIFOSIZ + TXH_P_FS_FIFOSIZ) > 640U)
#error  "The USB max FIFO size is 640 x 4 Bytes!"
#endif
#endif

#ifdef USB_HS_MODE
#define RX_FIFO_HS_SIZE                          (512U)
#define TXH_NP_HS_FIFOSIZ                        (128U)
#define TXH_P_HS_FIFOSIZ                         (256U)

#if ((RX_FIFO_FS_SIZE + TXH_NP_FS_FIFOSIZ + TXH_P_FS_FIFOSIZ) > 2048U)
#error  "The USB max FIFO size is 2048 x 4 Bytes!"
#endif
#endif

/* FUNCTION CONFIGURATION */
#define USBH_MAX_NUM_INTERFACES                  (3U)
#define USBH_MAX_NUM_ENDPOINTS                   (2U)

/* CONFIGURATION FOR MSC */
#define USBH_MSC_MPS_SIZE                        (0x40U)
//#define USB_MSC_FAT_VALID

#ifdef __cplusplus
}
#endif

#endif /* __USB_APP_CONF_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
