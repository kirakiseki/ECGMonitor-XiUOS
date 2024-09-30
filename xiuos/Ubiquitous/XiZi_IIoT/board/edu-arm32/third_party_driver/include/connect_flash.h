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
* @file connect_flash.h
* @brief define edu-arm32-board qspi-flash function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2022-10-17
*/

#ifndef CONNECT_FLASH_H
#define CONNECT_FLASH_H

#include <device.h>
#include <hardware_irq.h>
#include <flash_spi.h>
#include <hc32_ll_qspi.h>
#include <hc32_ll_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

int FlashW25qxxSpiDeviceInit(void);

#ifdef __cplusplus
}
#endif

#endif
