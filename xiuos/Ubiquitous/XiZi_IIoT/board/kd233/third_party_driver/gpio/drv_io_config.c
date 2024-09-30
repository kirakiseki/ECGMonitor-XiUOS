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
* @file drv_io_config.c
* @brief support kd233-board io configure
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-25
*/

/*************************************************
File name: drv_io_config.c
Description: support kd233-board io configure
Others: take RT-Thread v4.0.2/bsp/k210/driver/drv_io_config.c for references
                https://github.com/RT-Thread/rt-thread/tree/v4.0.2
History: 
1. Date: 2021-04-25
Author: AIIT XUOS Lab
Modification: support kd233-board io configure
*************************************************/

#include <xizi.h>
#include <fpioa.h>
#include "drv_io_config.h"
#include <sysctl.h>


//#define        LED_TEST             

#define HS_GPIO(n) (FUNC_GPIOHS0 + n)

#define IOCONFIG(pin,func)  {pin, func, #func}

static struct io_config
{
    int io_num;
    fpioa_function_t func;
    const char * FuncName;
} io_config[] = 
{
#ifdef BSP_USING_LCD
    IOCONFIG(BSP_LCD_CS_PIN, FUNC_SPI0_SS0),                
    IOCONFIG(BSP_LCD_WR_PIN, FUNC_SPI0_SCLK),                
    IOCONFIG(BSP_LCD_DC_PIN, HS_GPIO(LCD_DC_PIN)),     
#endif

#ifdef BSP_USING_CAMERA
    IOCONFIG(BSP_CAMERA_SCCB_SDA_PIN, FUNC_SCCB_SDA),
    IOCONFIG(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_SCCB_SCLK),
    IOCONFIG(BSP_CAMERA_CMOS_RST_PIN, FUNC_CMOS_RST),
    IOCONFIG(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_CMOS_VSYNC),
    IOCONFIG(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_CMOS_PWDN),
    IOCONFIG(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_CMOS_XCLK),
    IOCONFIG(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_CMOS_PCLK),
    IOCONFIG(BSP_CAMERA_CMOS_HREF_PIN, FUNC_CMOS_HREF),
#endif

#ifdef BSP_USING_SPI1
    IOCONFIG(BSP_SPI1_CLK_PIN, FUNC_SPI1_SCLK),
    IOCONFIG(BSP_SPI1_D0_PIN, FUNC_SPI1_D0),
    IOCONFIG(BSP_SPI1_D1_PIN, FUNC_SPI1_D1),
#ifdef BSP_USING_SPI1_AS_QSPI
    IOCONFIG(BSP_SPI1_D2_PIN, FUNC_SPI1_D2),
    IOCONFIG(BSP_SPI1_D3_PIN, FUNC_SPI1_D3),
#endif
#ifdef BSP_SPI1_USING_SS0
    IOCONFIG(BSP_SPI1_SS0_PIN, HS_GPIO(SPI1_CS0_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS1
    IOCONFIG(BSP_SPI1_SS1_PIN, HS_GPIO(SPI1_CS1_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS2
    IOCONFIG(BSP_SPI1_SS2_PIN, HS_GPIO(SPI1_CS2_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS3
    IOCONFIG(BSP_SPI1_SS3_PIN, HS_GPIO(SPI1_CS3_PIN)),
#endif
#endif

#ifdef BSP_USING_UART1
    IOCONFIG(BSP_UART1_TXD_PIN, FUNC_UART1_TX),
    IOCONFIG(BSP_UART1_RXD_PIN, FUNC_UART1_RX),
#endif
#ifdef BSP_USING_UART2
    IOCONFIG(BSP_UART2_TXD_PIN, FUNC_UART2_TX),
    IOCONFIG(BSP_UART2_RXD_PIN, FUNC_UART2_RX),
#endif
#ifdef BSP_USING_UART3
    IOCONFIG(BSP_UART3_TXD_PIN, FUNC_UART3_TX),
    IOCONFIG(BSP_UART3_RXD_PIN, FUNC_UART3_RX),
#endif

// #ifdef LED_TEST
//     IOCONFIG(LED0, FUNC_GPIOHS0),
// #endif
    IOCONFIG(18, FUNC_GPIOHS18),
    IOCONFIG(19, FUNC_GPIOHS19),
};

static int PrintIoConfig()
{
    int i;
    KPrintf("IO Configuration Table\n");
    KPrintf("┌───────┬────────────────────────┐\n");
    KPrintf("│Pin    │Function                │\n");
    KPrintf("├───────┼────────────────────────┤\n");
    for(i = 0; i < sizeof io_config / sizeof io_config[0]; i++)
    {
        KPrintf("│%-2d     │%-24.24s│\n", io_config[i].io_num, io_config[i].FuncName);
    }
    KPrintf("└───────┴────────────────────────┘\n");
    return 0;
}



SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0),
                                                io,PrintIoConfig,print io config);


int IoConfigInit(void)
{
    int count = sizeof(io_config) / sizeof(io_config[0]);
    int i;
    int ret = 0;

    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
#ifdef BSP_USING_UART2
    // for IO-27/28
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, SYSCTL_POWER_V33);
#endif
#if  defined(BSP_USING_UART1) || defined(BSP_USING_UART3)
    // for IO-20~23
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, SYSCTL_POWER_V33);
#endif

    for(i = 0; i < count; i++)
    {
        ret = FpioaSetFunction(io_config[i].io_num, io_config[i].func);
        if(ret != 0)
            return ret;
    }

#if defined(BSP_USING_CAMERA) || defined(BSP_USING_LCD)
    sysctl_set_spi0_dvp_data(1);
#endif

    return ret;
}

