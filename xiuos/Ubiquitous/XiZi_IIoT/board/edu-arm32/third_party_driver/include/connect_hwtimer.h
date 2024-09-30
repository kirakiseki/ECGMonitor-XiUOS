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
* @file connect_hwtimer.h
* @brief define edu-arm32-board hwtimer function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2023-02-16
*/

#ifndef CONNECT_HWTIMER_H
#define CONNECT_HWTIMER_H

#include <device.h>
#include <hc32f4a0.h>
#include <hardware_irq.h>
#include <hc32_ll_tmr0.h>
#include <hc32_ll_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

int HwTimerInit(void);

#ifdef __cplusplus
}
#endif

#endif
