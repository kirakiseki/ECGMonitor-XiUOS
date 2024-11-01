/*
* Copyright (c) 2021 AIIT XUOS Lab
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
* @file connect_can.h
* @brief define edu-arm32-board can function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2023-02-21
*/

#ifndef CONNECT_CAN_H
#define CONNECT_CAN_H

#include <device.h>
#include <hc32_ll_can.h>
#include <hc32_ll_clk.h>
#include <hc32_ll_gpio.h>
#include <hardware_irq.h>

#ifdef __cplusplus
 extern "C" {
#endif

int HwCanInit(void);

#ifdef __cplusplus
}
#endif

#endif
