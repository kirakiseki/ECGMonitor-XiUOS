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
* @file connect_gpio.h
* @brief define edu-arm32-board gpio function and struct
* @version 3.0
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

#ifndef CONNECT_GPIO_H
#define CONNECT_GPIO_H

#include <device.h>
#include <hc32_ll.h>
#include <hc32_ll_gpio.h>
#include <hc32_ll_utility.h>
#include <hardware_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Hc32PinIrqMap
{
    uint16_t                pinbit;
    func_ptr_t              irq_callback;
    struct Hc32IrqConfig    irq_config;
};

#ifndef EXTINT0_IRQ_CONFIG
#define EXTINT0_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT0_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT0_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ0,                   \
    }
#endif /* EXTINT1_IRQ_CONFIG */

#ifndef EXTINT1_IRQ_CONFIG
#define EXTINT1_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT1_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT1_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ1,                   \
    }
#endif /* EXTINT1_IRQ_CONFIG */

#ifndef EXTINT2_IRQ_CONFIG
#define EXTINT2_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT2_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT2_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ2,                   \
    }
#endif /* EXTINT2_IRQ_CONFIG */

#ifndef EXTINT3_IRQ_CONFIG
#define EXTINT3_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT3_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT3_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ3,                   \
    }
#endif /* EXTINT3_IRQ_CONFIG */

#ifndef EXTINT4_IRQ_CONFIG
#define EXTINT4_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT4_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT4_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ4,                   \
    }
#endif /* EXTINT4_IRQ_CONFIG */

#ifndef EXTINT5_IRQ_CONFIG
#define EXTINT5_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT5_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT5_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ5,                   \
    }
#endif /* EXTINT5_IRQ_CONFIG */

#ifndef EXTINT6_IRQ_CONFIG
#define EXTINT6_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT6_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT6_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ6,                   \
    }
#endif /* EXTINT6_IRQ_CONFIG */

#ifndef EXTINT7_IRQ_CONFIG
#define EXTINT7_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT7_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT7_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ7,                   \
    }
#endif /* EXTINT7_IRQ_CONFIG */

#ifndef EXTINT8_IRQ_CONFIG
#define EXTINT8_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT8_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT8_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ8,                   \
    }
#endif /* EXTINT8_IRQ_CONFIG */

#ifndef EXTINT9_IRQ_CONFIG
#define EXTINT9_IRQ_CONFIG                                  \
    {                                                       \
        .irq_num    = BSP_EXTINT9_IRQ_NUM,                  \
        .irq_prio   = BSP_EXTINT9_IRQ_PRIO,                 \
        .int_src    = INT_SRC_PORT_EIRQ9,                   \
    }
#endif /* EXTINT9_IRQ_CONFIG */

#ifndef EXTINT10_IRQ_CONFIG
#define EXTINT10_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT10_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT10_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ10,                  \
    }
#endif /* EXTINT10_IRQ_CONFIG */

#ifndef EXTINT11_IRQ_CONFIG
#define EXTINT11_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT11_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT11_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ11,                  \
    }
#endif /* EXTINT11_IRQ_CONFIG */

#ifndef EXTINT12_IRQ_CONFIG
#define EXTINT12_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT12_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT12_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ12,                  \
    }
#endif /* EXTINT12_IRQ_CONFIG */

#ifndef EXTINT13_IRQ_CONFIG
#define EXTINT13_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT13_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT13_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ13,                  \
    }
#endif /* EXTINT13_IRQ_CONFIG */

#ifndef EXTINT14_IRQ_CONFIG
#define EXTINT14_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT14_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT14_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ14,                  \
    }
#endif /* EXTINT14_IRQ_CONFIG */

#ifndef EXTINT15_IRQ_CONFIG
#define EXTINT15_IRQ_CONFIG                                 \
    {                                                       \
        .irq_num    = BSP_EXTINT15_IRQ_NUM,                 \
        .irq_prio   = BSP_EXTINT15_IRQ_PRIO,                \
        .int_src    = INT_SRC_PORT_EIRQ15,                  \
    }
#endif /* EXTINT15_IRQ_CONFIG */

int HwGpioInit(void);

#ifdef __cplusplus
}
#endif

#endif
