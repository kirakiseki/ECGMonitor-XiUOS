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
 * @file fins.h
 * @brief plc protocol fins 
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-10-08
 */

#ifndef FINS_H
#define FINS_H

#include <control_def.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FINS_HEADER_HEAD                0x46494E53
#define FINS_HEADER_READ_COMMAND_LENGTH 0x0000001A
#define FINS_HEADER_COMMAND             0x00000002
#define FINS_HEADER_ERROR_CODE          0x00000000
#define FINS_ICF                        0x80
#define FINS_RSV                        0x00
#define FINS_REPLY_ICF                  0xC0
#define FINS_GCT                        0x02
#define FINS_DNA                        0x00
#define FINS_DA2                        0x00
#define FINS_SNA                        0x00
#define FINS_SA2                        0x00
#define FINS_SID                        0x00

typedef enum 
{
    FINS_COMMAND_CODE_READ = 0x0101,
    FINS_COMMAND_CODE_WRITE = 0x0102
}FinsCommandCode;

typedef enum 
{
    FINS_DATA_TYPE_BIT,
    FINS_DATA_TYPE_WORD
}FinsDataType;

typedef struct 
{
    BasicPlcDataInfo base_data_info;
    FinsCommandCode command_code;
}FinsDataInfo;

typedef struct 
{
    int32_t socket;
    uint16_t plc_ip_4;
}FinsHandshakeParameter;

typedef struct
{
    FinsDataInfo data_info;

    UniformValueType value_type;
    uint8_t value_name[20];
    
    uint8_t area_char;
    FinsDataType data_type;
    uint16_t start_address;
    uint8_t bit_address;
    uint16_t data_length;
}FinsReadItem;

#ifdef __cplusplus
}
#endif

#endif
