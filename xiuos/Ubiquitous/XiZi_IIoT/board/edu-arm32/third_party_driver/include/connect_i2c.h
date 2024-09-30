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
* @file connect_i2c.h
* @brief define edu-arm32-board i2c function and struct
* @version 3.0
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

#ifndef CONNECT_I2C_H
#define CONNECT_I2C_H

#include <device.h>
#include <hc32_ll.h>
#include <hc32_ll_i2c.h>
#include <hc32_ll_utility.h>
#include <hc32_ll_fcg.h>
#include <hc32_ll_gpio.h>
#include <hc32_ll_def.h>

#ifdef __cplusplus
extern "C" {
#endif

int HwI2cInit(void);

#ifdef __cplusplus
}
#endif

#endif
