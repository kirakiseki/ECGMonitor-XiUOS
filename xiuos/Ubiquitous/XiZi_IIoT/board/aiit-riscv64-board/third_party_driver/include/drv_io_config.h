/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-19     ZYH          first version
 */

/**
* @file drv_io_config.h
* @brief define aiit-riscv64-board io configure
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-25
*/

/*************************************************
File name: drv_io_config.h
Description: define aiit-riscv64-board io configure
Others: take RT-Thread v4.0.2/bsp/k210/driver/drv_io_config.h for references
                https://github.com/RT-Thread/rt-thread/tree/v4.0.2
History: 
1. Date: 2021-04-25
Author: AIIT XUOS Lab
Modification: add aiit-riscv64-board io configure define
*************************************************/

#ifndef __DRV_IO_CONFIG_H__
#define __DRV_IO_CONFIG_H__

enum HS_GPIO_CONFIG
{
#ifdef BSP_USING_LCD
    LCD_DC_PIN = 0,     /* LCD DC PIN */
#endif
#ifdef BSP_SPI1_USING_SS0
    SPI1_CS0_PIN,
#endif
#ifdef BSP_SPI1_USING_SS1
    SPI1_CS1_PIN,
#endif
#ifdef BSP_SPI1_USING_SS2
    SPI1_CS2_PIN,
#endif
#ifdef BSP_SPI1_USING_SS3
    SPI1_CS3_PIN,
#endif
    GPIO_ALLOC_START /* index of gpio driver start */
};

#ifdef BSP_USING_CH438
#define FPIOA_CH438_ALE   12
#define FPIOA_CH438_NWR   13
#define FPIOA_CH438_NRD   14
#define FPIOA_CH438_D0    15
#define FPIOA_CH438_D1    16
#define FPIOA_CH438_D2    17
#define FPIOA_CH438_D3    18
#define FPIOA_CH438_D4    19
#define FPIOA_CH438_D5    20
#define FPIOA_CH438_D6    21
#define FPIOA_CH438_D7    22
#define FPIOA_CH438_INT   23

#define BSP_CH438_ALE_PIN   24
#define BSP_CH438_NWR_PIN   25
#define BSP_CH438_NRD_PIN   26
#define BSP_CH438_D0_PIN    27
#define BSP_CH438_D1_PIN    28
#define BSP_CH438_D2_PIN    29
#define BSP_CH438_D3_PIN    30
#define BSP_CH438_D4_PIN    31
#define BSP_CH438_D5_PIN    32
#define BSP_CH438_D6_PIN    33
#define BSP_CH438_D7_PIN    34
#define BSP_CH438_INT_PIN   35
#endif

extern int IoConfigInit(void);

#endif
