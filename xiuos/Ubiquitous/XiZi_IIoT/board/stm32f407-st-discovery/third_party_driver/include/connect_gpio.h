/*
* Copyright (c) 2022 AIIT XUOS Lab
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
* @brief define stm32f407-st-discovery-board gpio function and struct
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-03-01
*/

#ifndef __CONNECT_GPIO_H_
#define __CONNECT_GPIO_H_

#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

int Stm32HwGpioInit(void);

#ifdef __cplusplus
}
#endif

#endif