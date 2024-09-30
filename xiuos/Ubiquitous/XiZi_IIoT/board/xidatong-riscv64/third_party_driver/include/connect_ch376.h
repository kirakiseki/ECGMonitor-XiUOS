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
* @file connect_ch376.h
* @brief define xidatong-riscv64 ch376 function and struct
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-28
*/

#ifndef CONNECT_CH376_H
#define CONNECT_CH376_H

#include <device.h>

struct HwCh376
{
    HardwareDevType dev;
    x_size_t msg_len;
    int sem;
    KTaskDescriptorType task;
};

int HwCh376Init(void);

#endif
