/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_bsp.h
 * @brief Head file for usb_bsp.c
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
* @file usb_bsp.h
* @brief support edu-arm32-board usb bsp function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-11-08
*/

/*************************************************
File name: usb_bsp.h
Description: support edu-arm32-board usb bsp function
Others: 
History: 
1. Date: 2022-11-08
Author: AIIT XUOS Lab
Modification: 
*************************************************/

#ifndef USB_BSP_H
#define USB_BSP_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <usb_lib.h>

/**
 * @addtogroup HC32F4A0_DDL_Applications
 * @{
 */

/**
 * @addtogroup USB_Host_Msc
 * @{
 */

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/


/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
extern void usb_bsp_init(usb_core_instance *pdev);
extern void usb_udelay(const uint32_t usec);
extern void usb_mdelay(const uint32_t msec);
extern void usb_bsp_nvicconfig(void);
#ifdef USE_HOST_MODE
extern void usb_bsp_cfgvbus(LL_USB_TypeDef *USBx);
extern void usb_bsp_drivevbus(LL_USB_TypeDef *USBx, uint8_t state);
#endif

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __USB_BSP_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
