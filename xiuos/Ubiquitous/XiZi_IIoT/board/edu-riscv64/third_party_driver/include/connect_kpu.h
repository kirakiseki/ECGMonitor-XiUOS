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
* @file connect_dvp.h
* @brief define edu-riscv64-board DVP init function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-11-21
*/
#ifndef CONNECT_DVP_H
#define CONNECT_DVP_H
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t *buffer;
    size_t length;
}KpuOutputBuffer;


int HwKpuInit(void);

#ifdef __cplusplus
}
#endif

#endif