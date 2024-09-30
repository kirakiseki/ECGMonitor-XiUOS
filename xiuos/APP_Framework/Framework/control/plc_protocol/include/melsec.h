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
 * @file melsec.h
 * @brief plc protocol melsec
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-11-29
 */

#ifndef MELSEC_H
#define MELSEC_H

#include <control_def.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUB_HEADER                                0x5000
#define NETWORK_NUMBER                            0x00
#define PC_NUMBER                                 0xFF
#define QEQUEST_DESTINSTION_MODULE_IO_NUMBER      0x03FF
#define QEQUEST_DESTINSTION_MODULE_STATION_NUMBER 0x00
#define STATION_NUMBER                            0x00
#define FRAME_NUMBER                              0x4639
#define SELF_STATION_NUMBER                       0x00

#define MELSEC_NAK 0x15
#define MELSEC_STX 0x02
#define MELSEC_ETX 0x03
#define MELSEC_ENQ 0x05

#define MELSEC_1E_FRAME_RB 0x00
#define MELSEC_1E_FRAME_RW 0x01
#define MELSEC_1E_FRAME_WB 0x02
#define MELSEC_1E_FRAME_WW 0x03

#define MELSEC_1C_FRAME_RB 0x4252
#define MELSEC_1C_FRAME_RW 0x5752
#define MELSEC_1C_FRAME_WB 0x4257
#define MELSEC_1C_FRAME_WW 0x5757

//same as MELSEC_3E_Q_L_FRAME
#define MELSEC_3C_FRAME_RB 0x04010001
#define MELSEC_3C_FRAME_RW 0x04010000
#define MELSEC_3C_FRAME_WB 0x14010001
#define MELSEC_3C_FRAME_WW 0x14010000

//same as MELSEC_3C_FRAME
#define MELSEC_3E_Q_L_FRAME_RB 0x04010001
#define MELSEC_3E_Q_L_FRAME_RW 0x04010000
#define MELSEC_3E_Q_L_FRAME_WB 0x14010001
#define MELSEC_3E_Q_L_FRAME_WW 0x14010000

#define MELSEC_3E_IQ_R_FRAME_RB 0x04010003
#define MELSEC_3E_IQ_R_FRAME_RW 0x04010002
#define MELSEC_3E_IQ_R_FRAME_WB 0x14010003
#define MELSEC_3E_IQ_R_FRAME_WW 0x14010002

typedef enum {
    READ_IN_BITS,
    READ_IN_WORD,
    WRITE_IN_BITS,
    WRITE_IN_WORD,
    TEST_IN_BIT,
    TEST_IN_WORD
}MelsecCommandType;

typedef enum {
    MELSEC_1E_FRAME,
    MELSEC_3E_Q_L_FRAME,
    MELSEC_3E_IQ_R_FRAME,
    MELSEC_1C_FRAME,
    MELSEC_3C_FRAME
}MelsecFrameType;

typedef struct 
{
    BasicPlcDataInfo base_data_info;
    MelsecCommandType command_type;
    MelsecFrameType frame_type;
}MelsecDataInfo;

typedef struct
{
    MelsecDataInfo data_info;

    UniformValueType value_type;
    uint8_t value_name[20];
    
    uint16_t monitoring_timer;
    uint16_t device_code;
    uint8_t head_device_number_string[6];
    uint16_t device_points_count;
}MelsecReadItem;

#ifdef __cplusplus
}
#endif

#endif
