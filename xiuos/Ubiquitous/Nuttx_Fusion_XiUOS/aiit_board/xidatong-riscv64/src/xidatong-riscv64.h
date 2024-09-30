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
 * @file xidatong-riscv64.h
 * @brief xidatong-riscv64 xidatong-riscv64.h
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */

#ifndef __BOARDS_RISCV_K210_XIDATONG_RISCV64_SRC_XIDATONG_RISCV64_H
#define __BOARDS_RISCV_K210_XIDATONG_RISCV64_SRC_XIDATONG_RISCV64_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

int k210_bringup(void);

#ifdef CONFIG_DEV_GPIO
int k210_gpio_init(void);
#endif

#endif /* __BOARDS_RISCV_K210_XIDATONG_RISCV64_SRC_XIDATONG_RISCV64_H */
