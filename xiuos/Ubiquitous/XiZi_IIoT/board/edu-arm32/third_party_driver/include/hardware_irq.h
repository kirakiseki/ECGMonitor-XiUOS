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
* @file hardware_irq.h
* @brief define edu-arm32-board irq function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2022-09-13
*/

#ifndef __HARDWARE_IRQ_H__
#define __HARDWARE_IRQ_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <device.h>
#include <hc32_ll.h>

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
struct Hc32IrqConfig
{
    IRQn_Type       irq_num;
    uint32          irq_prio;
    en_int_src_t    int_src;
};

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BSP_EXTINT0_IRQ_NUM             INT022_IRQn
#define BSP_EXTINT0_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT1_IRQ_NUM             INT023_IRQn
#define BSP_EXTINT1_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT2_IRQ_NUM             INT024_IRQn
#define BSP_EXTINT2_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT3_IRQ_NUM             INT025_IRQn
#define BSP_EXTINT3_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT4_IRQ_NUM             INT026_IRQn
#define BSP_EXTINT4_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT5_IRQ_NUM             INT027_IRQn
#define BSP_EXTINT5_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT6_IRQ_NUM             INT028_IRQn
#define BSP_EXTINT6_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT7_IRQ_NUM             INT029_IRQn
#define BSP_EXTINT7_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT8_IRQ_NUM             INT030_IRQn
#define BSP_EXTINT8_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT9_IRQ_NUM             INT031_IRQn
#define BSP_EXTINT9_IRQ_PRIO            DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT10_IRQ_NUM            INT032_IRQn
#define BSP_EXTINT10_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT11_IRQ_NUM            INT033_IRQn
#define BSP_EXTINT11_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT12_IRQ_NUM            INT034_IRQn
#define BSP_EXTINT12_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT13_IRQ_NUM            INT035_IRQn
#define BSP_EXTINT13_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT14_IRQ_NUM            INT036_IRQn
#define BSP_EXTINT14_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_EXTINT15_IRQ_NUM            INT037_IRQn
#define BSP_EXTINT15_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

/* DMA1 ch0 */
#define BSP_DMA1_CH0_IRQ_NUM            INT038_IRQn
#define BSP_DMA1_CH0_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch1 */
#define BSP_DMA1_CH1_IRQ_NUM            INT039_IRQn
#define BSP_DMA1_CH1_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch2 */
#define BSP_DMA1_CH2_IRQ_NUM            INT040_IRQn
#define BSP_DMA1_CH2_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch3 */
#define BSP_DMA1_CH3_IRQ_NUM            INT041_IRQn
#define BSP_DMA1_CH3_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch4 */
#define BSP_DMA1_CH4_IRQ_NUM            INT042_IRQn
#define BSP_DMA1_CH4_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch5 */
#define BSP_DMA1_CH5_IRQ_NUM            INT043_IRQn
#define BSP_DMA1_CH5_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch6 */
#define BSP_DMA1_CH6_IRQ_NUM            INT018_IRQn
#define BSP_DMA1_CH6_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA1 ch7 */
#define BSP_DMA1_CH7_IRQ_NUM            INT019_IRQn
#define BSP_DMA1_CH7_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

/* DMA2 ch0 */
#define BSP_DMA2_CH0_IRQ_NUM            INT044_IRQn
#define BSP_DMA2_CH0_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch1 */
#define BSP_DMA2_CH1_IRQ_NUM            INT045_IRQn
#define BSP_DMA2_CH1_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch2 */
#define BSP_DMA2_CH2_IRQ_NUM            INT046_IRQn
#define BSP_DMA2_CH2_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch3 */
#define BSP_DMA2_CH3_IRQ_NUM            INT047_IRQn
#define BSP_DMA2_CH3_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch4 */
#define BSP_DMA2_CH4_IRQ_NUM            INT048_IRQn
#define BSP_DMA2_CH4_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch5 */
#define BSP_DMA2_CH5_IRQ_NUM            INT049_IRQn
#define BSP_DMA2_CH5_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch6 */
#define BSP_DMA2_CH6_IRQ_NUM            INT020_IRQn
#define BSP_DMA2_CH6_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
/* DMA2 ch7 */
#define BSP_DMA2_CH7_IRQ_NUM            INT021_IRQn
#define BSP_DMA2_CH7_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

#if defined(BSP_USING_ETH)
#define BSP_ETH_IRQ_NUM                 INT104_IRQn
#define BSP_ETH_IRQ_PRIO                DDL_IRQ_PRIO_DEFAULT
#endif

#if defined(BSP_USING_UART1)
#define BSP_UART1_RXERR_IRQ_NUM         INT010_IRQn
#define BSP_UART1_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART1_RX_IRQ_NUM            INT089_IRQn
#define BSP_UART1_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART1_TX_IRQ_NUM            INT088_IRQn
#define BSP_UART1_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

#if defined(BSP_UART1_RX_USING_DMA)
#define BSP_UART1_RXTO_IRQ_NUM          INT006_IRQn
#define BSP_UART1_RXTO_IRQ_PRIO         DDL_IRQ_PRIO_DEFAULT
#endif
#if defined(BSP_UART1_TX_USING_DMA)
#define BSP_UART1_TX_CPLT_IRQ_NUM       INT086_IRQn
#define BSP_UART1_TX_CPLT_IRQ_PRIO      DDL_IRQ_PRIO_DEFAULT
#endif
#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
#define BSP_UART2_RXERR_IRQ_NUM         INT011_IRQn
#define BSP_UART2_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART2_RX_IRQ_NUM            INT091_IRQn
#define BSP_UART2_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART2_TX_IRQ_NUM            INT090_IRQn
#define BSP_UART2_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

#if defined(BSP_UART2_RX_USING_DMA)
#define BSP_UART2_RXTO_IRQ_NUM          INT007_IRQn
#define BSP_UART2_RXTO_IRQ_PRIO         DDL_IRQ_PRIO_DEFAULT
#endif
#if defined(BSP_UART2_TX_USING_DMA)
#define BSP_UART2_TX_CPLT_IRQ_NUM       INT087_IRQn
#define BSP_UART2_TX_CPLT_IRQ_PRIO      DDL_IRQ_PRIO_DEFAULT
#endif
#endif /* BSP_USING_UART2 */

#if defined(BSP_USING_UART3)
#define BSP_UART3_RXERR_IRQ_NUM         INT012_IRQn
#define BSP_UART3_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART3_RX_IRQ_NUM            INT095_IRQn
#define BSP_UART3_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART3_TX_IRQ_NUM            INT094_IRQn
#define BSP_UART3_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART3 */

#if defined(BSP_USING_UART4)
#define BSP_UART4_RXERR_IRQ_NUM         INT013_IRQn
#define BSP_UART4_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART4_RX_IRQ_NUM            INT097_IRQn
#define BSP_UART4_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART4_TX_IRQ_NUM            INT096_IRQn
#define BSP_UART4_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART4 */

#if defined(BSP_USING_UART5)
#define BSP_UART5_RXERR_IRQ_NUM         INT014_IRQn
#define BSP_UART5_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART5_RX_IRQ_NUM            INT101_IRQn
#define BSP_UART5_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART5_TX_IRQ_NUM            INT100_IRQn
#define BSP_UART5_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART5 */

#if defined(BSP_USING_UART6)
#define BSP_UART6_RXERR_IRQ_NUM         INT015_IRQn
#define BSP_UART6_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART6_RX_IRQ_NUM            INT103_IRQn
#define BSP_UART6_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART6_TX_IRQ_NUM            INT102_IRQn
#define BSP_UART6_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

#if defined(BSP_UART6_RX_USING_DMA)
#define BSP_UART6_RXTO_IRQ_NUM          INT008_IRQn
#define BSP_UART6_RXTO_IRQ_PRIO         DDL_IRQ_PRIO_DEFAULT
#endif
#if defined(BSP_UART6_TX_USING_DMA)
#define BSP_UART6_TX_CPLT_IRQ_NUM       INT099_IRQn
#define BSP_UART6_TX_CPLT_IRQ_PRIO      DDL_IRQ_PRIO_DEFAULT
#endif
#endif /* BSP_USING_UART6 */

#if defined(BSP_USING_UART7)
#define BSP_UART7_RXERR_IRQ_NUM         INT016_IRQn
#define BSP_UART7_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART7_RX_IRQ_NUM            INT107_IRQn
#define BSP_UART7_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART7_TX_IRQ_NUM            INT106_IRQn
#define BSP_UART7_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT

#if defined(BSP_UART7_RX_USING_DMA)
#define BSP_UART7_RXTO_IRQ_NUM          INT009_IRQn
#define BSP_UART7_RXTO_IRQ_PRIO         DDL_IRQ_PRIO_DEFAULT
#endif
#if defined(BSP_UART7_TX_USING_DMA)
#define BSP_UART7_TX_CPLT_IRQ_NUM       INT105_IRQn
#define BSP_UART7_TX_CPLT_IRQ_PRIO      DDL_IRQ_PRIO_DEFAULT
#endif
#endif /* BSP_USING_UART7 */

#if defined(BSP_USING_UART8)
#define BSP_UART8_RXERR_IRQ_NUM         INT017_IRQn
#define BSP_UART8_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART8_RX_IRQ_NUM            INT109_IRQn
#define BSP_UART8_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART8_TX_IRQ_NUM            INT108_IRQn
#define BSP_UART8_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART8 */

#if defined(BSP_USING_UART9)
#define BSP_UART9_RXERR_IRQ_NUM         INT112_IRQn
#define BSP_UART9_RXERR_IRQ_PRIO        DDL_IRQ_PRIO_DEFAULT
#define BSP_UART9_RX_IRQ_NUM            INT110_IRQn
#define BSP_UART9_RX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#define BSP_UART9_TX_IRQ_NUM            INT111_IRQn
#define BSP_UART9_TX_IRQ_PRIO           DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART9 */

#if defined(BSP_USING_UART10)
#define BSP_UART10_RXERR_IRQ_NUM        INT115_IRQn
#define BSP_UART10_RXERR_IRQ_PRIO       DDL_IRQ_PRIO_DEFAULT
#define BSP_UART10_RX_IRQ_NUM           INT114_IRQn
#define BSP_UART10_RX_IRQ_PRIO          DDL_IRQ_PRIO_DEFAULT
#define BSP_UART10_TX_IRQ_NUM           INT113_IRQn
#define BSP_UART10_TX_IRQ_PRIO          DDL_IRQ_PRIO_DEFAULT
#endif /* BSP_USING_UART10 */

#if defined(BSP_USING_CAN1)
#define BSP_CAN1_IRQ_NUM                INT004_IRQn
#define BSP_CAN1_IRQ_PRIO               DDL_IRQ_PRIO_DEFAULT
#endif/* BSP_USING_CAN1 */

#if defined(BSP_USING_CAN2)
#define BSP_CAN2_IRQ_NUM                INT005_IRQn
#define BSP_CAN2_IRQ_PRIO               DDL_IRQ_PRIO_DEFAULT
#endif/* BSP_USING_CAN2 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/
x_err_t hc32_install_irq_handler(struct Hc32IrqConfig *irq_config,
                                  void (*irq_hdr)(void),
                                  x_bool irq_enable);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_IRQ_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
