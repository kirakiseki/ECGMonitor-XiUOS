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
* @file connect_extmem.h
* @brief declare aiit-arm32-board extmem fsmc function
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-05-28
*/

#ifndef CONNECT_EXTMEM_H
#define CONNECT_EXTMEM_H

#include <xsconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

int HwSramInit(void);

#ifdef __cplusplus
}
#endif

#endif