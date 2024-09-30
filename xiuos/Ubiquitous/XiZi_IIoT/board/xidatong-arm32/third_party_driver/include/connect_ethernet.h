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
* @file connect_ethernet.h
* @brief Adapted network software protocol stack and hardware operation functions
* @version 1.0
* @author AIIT XUOS Lab
* @date 2021-12-7
*/

#ifndef __CONNECT_ETHERNET_H_
#define __CONNECT_ETHERNET_H_

#include "enet_ethernetif.h"
#include "enet_ethernetif_priv.h"

#ifdef __cplusplus
 extern "C" {
#endif


#ifndef sourceClock
#define sourceClock CLOCK_GetFreq(kCLOCK_CoreSysClk)
#endif


#ifdef __cplusplus
}
#endif

#endif

