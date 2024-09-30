/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 */

/**
* @file hardware_irq.c
* @brief support edu-arm32-board irq configure
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-13
*/

/*************************************************
File name: hardware_irq.c
Description: support edu-arm32-board irq configure
Others: 
History: 
1. Date: 2022-09-13
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board irq
*************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <hardware_irq.h>

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
x_err_t hc32_install_irq_handler(struct Hc32IrqConfig *irq_config,
                                  void (*irq_hdr)(void),
                                  x_bool irq_enable)
{
    x_err_t result = -ERROR;
    stc_irq_signin_config_t stcIrqSignConfig;

    NULL_PARAM_CHECK(irq_config);
    NULL_PARAM_CHECK(irq_hdr);

    stcIrqSignConfig.enIRQn      = irq_config->irq_num;
    stcIrqSignConfig.enIntSrc    = irq_config->int_src;
    stcIrqSignConfig.pfnCallback = irq_hdr;
    if (LL_OK == INTC_IrqSignIn(&stcIrqSignConfig)) {
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, irq_config->irq_prio);
        if (irq_enable) {
            NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
        } else {
            NVIC_DisableIRQ(stcIrqSignConfig.enIRQn);
        }
        result = EOK;
    }
    return result;
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
