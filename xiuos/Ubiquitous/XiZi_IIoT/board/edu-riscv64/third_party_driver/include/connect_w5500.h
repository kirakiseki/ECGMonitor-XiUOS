/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v2 for more details.
 */

/**
 * @file connect_w5500.h
 * @brief define aiit-riscv64-board spi function and struct
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-10-16
 */

#ifndef _CONNECT_W5500_H_
#define _CONNECT_W5500_H_

#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct Bus *spi_bus;

int HwWiznetInit(void);

#ifdef __cplusplus
}
#endif

#endif
