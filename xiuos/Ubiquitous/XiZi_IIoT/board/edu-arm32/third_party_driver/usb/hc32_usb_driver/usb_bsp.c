/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_bsp.c
 * @brief BSP function for USB example
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
* @file usb_bsp.c
* @brief support edu-arm32-board usb bsp function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-11-08
*/

/*************************************************
File name: usb_bsp.c
Description: support edu-arm32-board usb bsp function
Others: 
History: 
1. Date: 2022-11-08
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board usb IO configure
2. support edu-arm32-board usb irq define
*************************************************/
#include <device.h>
#include "usb_bsp.h"
#include <usb_host_int.h>

extern usb_core_instance usb_app_instance;

/* USBFS Core*/
#define USB_DP_PORT                     (GPIO_PORT_A)
#define USB_DP_PIN                      (GPIO_PIN_12)

#define USB_DM_PORT                     (GPIO_PORT_A)
#define USB_DM_PIN                      (GPIO_PIN_11)

#define USB_DRVVBUS_PORT                (GPIO_PORT_C)
#define USB_DRVVBUS_PIN                 (GPIO_PIN_09)

#define USB_VBUSDET_PORT                (GPIO_PORT_A)
#define USB_VBUSDET_PIN                 (GPIO_PIN_09)

#define USB_OC_PORT                     (GPIO_PORT_D)
#define USB_OC_PIN                      (GPIO_PIN_15)

//USB HOST ISR
static void USB_IRQ_Handler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    usb_host_isr(&usb_app_instance);

    ENABLE_INTERRUPT(lock);
}

void usb_bsp_init(usb_core_instance *pdev)
{
    stc_gpio_init_t stcGpioCfg;

    /* USB clock source configurate */
    CLK_SetUSBClockSrc(CLK_USBCLK_SYSCLK_DIV5);

    (void)GPIO_StructInit(&stcGpioCfg);

#ifdef USB_FS_MODE
    stcGpioCfg.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(USB_DM_PORT, USB_DM_PIN, &stcGpioCfg);
    (void)GPIO_Init(USB_DP_PORT, USB_DP_PIN, &stcGpioCfg);

    GPIO_SetFunc(USB_DRVVBUS_PORT, USB_DRVVBUS_PIN, GPIO_FUNC_10); /* VBUS */

    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USBFS, ENABLE);
#endif
}

void usb_bsp_nvicconfig(void)
{
    stc_irq_signin_config_t stcIrqRegiConf;
    /* Register INT_SRC_USBFS_GLB Int to Vect.No.030 */
    stcIrqRegiConf.enIRQn = INT030_IRQn;
    /* Select interrupt function */
#ifdef USB_FS_MODE
    stcIrqRegiConf.enIntSrc = INT_SRC_USBFS_GLB;
#else
    stcIrqRegiConf.enIntSrc = INT_SRC_USBHS_GLB;
#endif
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &USB_IRQ_Handler;
    /* Registration IRQ */
    (void)INTC_IrqSignIn(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIO_15);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

void usb_udelay(const uint32_t usec)
{
    __IO uint32_t i;
    uint32_t j;
    j = (HCLK_VALUE + 1000000UL - 1UL) / 1000000UL * usec;
    for (i = 0UL; i < j; i++) {
    }
}

void usb_mdelay(const uint32_t msec)
{
    usb_udelay(msec * 1000);
}

/**
 * @brief  Configures the IO for the Vbus and OverCurrent
 * @param  [in] pdev        device instance
 * @retval None
 */
void usb_bsp_cfgvbus(LL_USB_TypeDef *USBx)
{
}

/**
 * @brief  Drive vbus
 * @param  [in] pdev        device instance
 * @param  [in] state       the vbus state it would be.
 * @retval None
 */
void usb_bsp_drivevbus(LL_USB_TypeDef *USBx, uint8_t state)
{
}
