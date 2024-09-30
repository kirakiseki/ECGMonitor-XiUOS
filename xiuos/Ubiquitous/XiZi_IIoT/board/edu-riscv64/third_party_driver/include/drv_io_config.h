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
 * @brief define edu-riscv64-board io configure
 * @version 2.0
 * @author AIIT XUOS Lab
 * @date 2022-10-17
 */

/*************************************************
File name: drv_io_config.h
Description: define edu-riscv64-board io configure
Others: take RT-Thread v4.0.2/bsp/k210/driver/drv_io_config.h for references
                https://github.com/RT-Thread/rt-thread/tree/v4.0.2
History:
1. Date: 2022-10-17
Author: AIIT XUOS Lab
Modification: add edu-riscv64-board io configure define
*************************************************/

#ifndef __DRV_IO_CONFIG_H__
#define __DRV_IO_CONFIG_H__

enum HS_GPIO_CONFIG {
#ifdef BSP_USING_LCD
    LCD_DC_PIN = 0, /* LCD DC PIN */
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
#ifdef BSP_USING_W5500
    WIZ_RST_PIN,
    WIZ_INT_PIN,
#endif
    GPIO_ALLOC_START /* index of gpio driver start */
}
;

#ifdef BSP_USING_SOFT_SPI
#define FPIOA_SOFT_SPI_SCK 26
#define FPIOA_SOFT_SPI_MIOS 25
#define FPIOA_SOFT_SPI_MSOI 27
#define FPIOA_SOFT_SPI_NCS 28
#endif


#ifdef BSP_USING_SOFT_SPI
#define FPIOA_SOFT_SPI_SCK 26
#define FPIOA_SOFT_SPI_MIOS 25
#define FPIOA_SOFT_SPI_MSOI 27
#define FPIOA_SOFT_SPI_NCS 28

#define BSP_SOFT_SPI_SCK_PIN 26
#define BSP_SOFT_SPI_MIOS_PIN 25
#define BSP_SOFT_SPI_MSOI_PIN 27
#define BSP_SOFT_SPI_NCS_PIN 28
#endif

#ifdef BSP_USING_CAMERA
#define BSP_DVP_RST_PIN 40
#define BSP_DVP_PWDN_PIN 41
#define BSP_DVP_XCLK_PIN 42
#define BSP_DVP_PCLK_PIN 43
#define BSP_DVP_HSYNC_PIN 44
#define BSP_DVP_VSYNC_PIN 45
#define BSP_DVP_SCL_PIN 46
#define BSP_DVP_SDA_PIN 47
#endif

#ifdef BSP_USING_LED
#define BSP_LED_PIN 29
#endif

#ifdef BSP_USING_KEY
#define BSP_KEY_PIN 31
#endif

#ifdef BSP_USING_LORA
#define BSP_E220_M0_PIN 32
#define BSP_E220_M1_PIN 33
#endif

#ifdef USER_TEST_RS485
#define BSP_485_DIR_PIN 24
#elif defined SENSOR_QS_FX
#define BSP_485_DIR_PIN 24
#elif defined SENSOR_QS_FS
#define BSP_485_DIR_PIN 24
#endif

    extern int
    IoConfigInit(void);

#endif
