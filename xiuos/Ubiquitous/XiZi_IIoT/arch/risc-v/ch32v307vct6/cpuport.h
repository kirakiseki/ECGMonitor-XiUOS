/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-09     WCH        the first version
 */
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

#ifndef CPUPORT_H__
#define CPUPORT_H__

/* bytes of register width  */
//#define ARCH_RISCV_FPU
#define ARCH_RISCV_FPU_S

#ifdef ARCH_CPU_64BIT
#define STORE                   sd
#define LOAD                    ld
#define StoreDS                 "sd"
#define LoadDS                  "ld"
#define REGBYTES                8
#define RegLength               8
#define RegLengthS              "8"
#else
#define STORE                   sw
#define LOAD                    lw
#define StoreDS                 "sw"
#define LoadDS                  "lw"
#define RegLength               4
#define REGBYTES                4
#define RegLengthS              "4"
#endif

/* FPU */
#ifdef ARCH_RISCV_FPU
#ifdef ARCH_RISCV_FPU_D
#define FSTORE                  fsd
#define FLOAD                   fld
#define FREGBYTES               8
#endif
#ifdef ARCH_RISCV_FPU_S
#define FSTORE                  fsw
#define FLOAD                   flw
#define FREGBYTES               4
#endif
#endif

#endif
