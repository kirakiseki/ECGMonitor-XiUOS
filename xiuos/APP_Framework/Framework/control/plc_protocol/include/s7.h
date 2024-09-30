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
 * @file s7.h
 * @brief plc protocol s7
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-10-08
 */

#ifndef S7_H
#define S7_H

#include <control_def.h>
#include "libs7.h"

#define BASE_PLC_RECV_BUFF_SIZE 1024

ControlProtocolType control_protocol;

int8_t ReadPlcDataByRecipe(struct ControlRecipe *p_recipe);
void voidpush(uint8_t *datastack,uint8_t* args,uint16_t length);
static uint8_t GetUniformValueTypeMemorySize(UniformValueType uniform_value_type);

typedef struct
{
    TS7DataItem data_info;
    UniformValueType value_type;
    char value_name[20];
}S7ReadItem;

#endif