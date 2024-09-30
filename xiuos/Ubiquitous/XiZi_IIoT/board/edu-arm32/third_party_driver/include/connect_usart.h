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
* @file connect_uart.h
* @brief define edu-arm32-board usart function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2022-09-13
*/

#ifndef CONNECT_UART_H
#define CONNECT_UART_H

#include <device.h>
#include <hardware_irq.h>
#include <hc32_ll_fcg.h>
#include <hc32_ll_usart.h>

#ifdef __cplusplus
extern "C" {
#endif

struct UsartIrqCfg
{
    struct Hc32IrqConfig irq_config;
    func_ptr_t irq_callback;
};

struct UsartHwCfg
{
    CM_USART_TypeDef *uart_device;
    uint32 usart_clock;
    struct UsartIrqCfg rx_err_irq;
    struct UsartIrqCfg rx_irq;
};

#define UART_BAUDRATE_ERR_MAX          (0.025F)
#define FCG_USART_CLK                  FCG_Fcg3PeriphClockCmd
#define FCG_TMR0_CLK                   FCG_Fcg2PeriphClockCmd
#define FCG_DMA_CLK                    FCG_Fcg0PeriphClockCmd

#define KERNEL_CONSOLE_BUS_NAME        SERIAL_BUS_NAME_3
#define KERNEL_CONSOLE_DRV_NAME        SERIAL_DRV_NAME_3
#define KERNEL_CONSOLE_DEVICE_NAME     SERIAL_3_DEVICE_NAME_0

int HwUsartInit(void);

#ifdef __cplusplus
}
#endif

#endif
