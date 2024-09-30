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
 * @file melsec.c
 * @brief plc protocol melsec, support 1E、3E_Q_L、3E_IQ_R、1C、3C
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-11-29
 */

#include <melsec.h>

static BasicSocketPlc plc_socket = {0};
static uint8_t recv_buff[1024] = {0};

/**
 * @description: Melsec Get Check Sum
 * @param p_command - p_command pointer
 * @param command_length - command length
 * @return checksum
 */
static uint8_t GetCheckSum(uint8_t *p_command, uint16_t command_length)
{
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < command_length; i++) {
        checksum += p_command[i];
    }
    return checksum;
}

/**
 * @description: Melsec Transform from Hex to Ascii
 * @param hex - hex
 * @return ascii
 */
static uint8_t TransformHexToAscii(uint8_t hex)
{
    hex %= 0x10;
    return hex < 0xA ? hex + '0' : hex - 10 + 'A';
}

/**
 * @description: Melsec Transform from Ascii to Hex
 * @param ascii - ascii
 * @return hex
 */
static uint8_t TransformAsciiToHex(uint8_t ascii)
{
    if (ascii > 'F' || ascii < '0' || (ascii > '9' && ascii < 'A'))
        return 0;
    else
        return ascii < 'A' ? ascii - '0' : ascii - 'A' + 10;
}

/**
 * @description: Melsec Get Device Code
 * @param frame_type - melsec frame type
 * @param device_string - device string
 * @return device code
 */
static int MelsecGetDeviceCode(MelsecFrameType frame_type, char device_string[2])
{
    switch (frame_type) {
    case MELSEC_1C_FRAME:
        if (strcmp(device_string, "M") == 0)
            return 0x4D;
        if (strcmp(device_string, "D") == 0)
            return 0x44;
        if (strcmp(device_string, "B") == 0)
            return 0x22;
        if (strcmp(device_string, "W") == 0)
            return 0x57;
        if (strcmp(device_string, "X") == 0)
            return 0x58;
        if (strcmp(device_string, "Y") == 0)
            return 0x59;
    case MELSEC_1E_FRAME:
        if (strcmp(device_string, "M") == 0)
            return 0x4D20;
        if (strcmp(device_string, "D") == 0)
            return 0x4420;
        if (strcmp(device_string, "B") == 0)
            return 0x2220;
        if (strcmp(device_string, "W") == 0)
            return 0x5720;
        if (strcmp(device_string, "X") == 0)
            return 0x5820;
        if (strcmp(device_string, "Y") == 0)
            return 0x5920;
    case MELSEC_3C_FRAME:
        if (strcmp(device_string, "M") == 0)
            return 0x4D2A;
        if (strcmp(device_string, "D") == 0)
            return 0x442A;
        if (strcmp(device_string, "B") == 0)
            return 0x222A;
        if (strcmp(device_string, "W") == 0)
            return 0x572A;
    case  MELSEC_3E_IQ_R_FRAME:
        if (strcmp(device_string, "M") == 0)
            return 0x0090;
        if (strcmp(device_string, "D") == 0)
            return 0x00A8;
        if (strcmp(device_string, "B") == 0)
            return 0x00A0;
        if (strcmp(device_string, "W") == 0)
            return 0x00B4;
        if (strcmp(device_string, "X") == 0)
            return 0x009C;
        if (strcmp(device_string, "Y") == 0)
            return 0x009D;
    case  MELSEC_3E_Q_L_FRAME:
        if (strcmp(device_string, "M") == 0)
            return 0x90;
        if (strcmp(device_string, "D") == 0)
            return 0xA8;
        if (strcmp(device_string, "B") == 0)
            return 0xA0;
        if (strcmp(device_string, "W") == 0)
            return 0xB4;
        if (strcmp(device_string, "X") == 0)
            return 0x9C;
        if (strcmp(device_string, "Y") == 0)
            return 0x9D;
    }
}

/**
 * @description: Melsec Get Command Base Length
 * @param frame_type - melsec frame type
 * @return command length
 */
static int MelsecGetCommandBaseLength(MelsecFrameType frame_type)
{
    switch (frame_type) {
    case MELSEC_1C_FRAME:
        return 17;
    case MELSEC_1E_FRAME:
        return 12;
    case MELSEC_3C_FRAME:
        return 33;
    case MELSEC_3E_IQ_R_FRAME:
    case MELSEC_3E_Q_L_FRAME:
        return 21;
    default:
        return -1;
    }
}

/**
 * @description: Melsec Get Command Code
 * @param frame_type - melsec frame type
 * @param command_type - melsec command type
 * @return command code
 */
static uint32_t MelsecGetCommandCode(MelsecFrameType frame_type, MelsecCommandType command_type)
{
    switch (frame_type) {
    case MELSEC_1C_FRAME:
        switch (command_type) {
        case READ_IN_BITS:
            return MELSEC_1C_FRAME_RB;
        case READ_IN_WORD:
            return MELSEC_1C_FRAME_RW;
        case WRITE_IN_BITS:
            return MELSEC_1C_FRAME_WB;
        case WRITE_IN_WORD:
            return MELSEC_1C_FRAME_WW;
        }
    case MELSEC_1E_FRAME:
        return command_type;
    case MELSEC_3C_FRAME:
    case MELSEC_3E_Q_L_FRAME:
        switch (command_type) {
        case READ_IN_BITS:
            return MELSEC_3E_Q_L_FRAME_RB;
        case READ_IN_WORD:
            return MELSEC_3E_Q_L_FRAME_RW;
        case WRITE_IN_BITS:
            return MELSEC_3E_Q_L_FRAME_WB;
        case WRITE_IN_WORD:
            return MELSEC_3E_Q_L_FRAME_WW;
        }
    case MELSEC_3E_IQ_R_FRAME:
        switch (command_type) {
        case READ_IN_BITS:
            return MELSEC_3E_IQ_R_FRAME_RB;
        case READ_IN_WORD:
            return MELSEC_3E_IQ_R_FRAME_RW;
        case WRITE_IN_BITS:
            return MELSEC_3E_IQ_R_FRAME_WB;
        case WRITE_IN_WORD:
            return MELSEC_3E_IQ_R_FRAME_WW;
        }
    }
}

/**
 * @description: Melsec_1E Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t Melsec1eGenerateCommand(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    uint16_t index = 0;

    p_command[index++] = command_code;
    p_command[index++] = PC_NUMBER;
    p_command[index++] = p_read_item->monitoring_timer / 250;
    p_command[index++] = (p_read_item->monitoring_timer / 250) >> 8;

    uint16_t head_device_number = 0;
    for (uint8_t i = 0; i < 6; i++) {
        if (0 != p_read_item->head_device_number_string[i])
            head_device_number = TransformAsciiToHex(p_read_item->head_device_number_string[i]) + head_device_number * (((0x5820 == p_read_item->device_code) || (0x5920 == p_read_item->device_code)) ? 8 : 10);
        else
            break;
    }
    p_command[index++] = head_device_number;
    p_command[index++] = head_device_number >> (8 * 1);
    p_command[index++] = head_device_number >> (8 * 2);
    p_command[index++] = head_device_number >> (8 * 3);
    p_command[index++] = p_read_item->device_code;
    p_command[index++] = p_read_item->device_code >> 8;
    p_command[index++] = p_read_item->device_points_count;
    p_command[index++] = 0x00;
    return index;
}

/**
 * @description: Melsec_3E_Q_L Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t Melsec3eqlGenerateCommand(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    p_read_item->monitoring_timer /= 250;
    uint16_t index = 0;

    p_command[index++] = SUB_HEADER >> 8;
    p_command[index++] = (uint8_t)SUB_HEADER;
    p_command[index++] = NETWORK_NUMBER;
    p_command[index++] = PC_NUMBER;
    p_command[index++] = (uint8_t)QEQUEST_DESTINSTION_MODULE_IO_NUMBER;
    p_command[index++] = (uint8_t)(QEQUEST_DESTINSTION_MODULE_IO_NUMBER >> 8);
    p_command[index++] = QEQUEST_DESTINSTION_MODULE_STATION_NUMBER;
    p_command[index++] = 0x0C;
    p_command[index++] = 0x00;
    p_command[index++] = p_read_item->monitoring_timer;
    p_command[index++] = p_read_item->monitoring_timer >> 8;
    p_command[index++] = command_code >> (8 * 2);
    p_command[index++] = command_code >> (8 * 3);
    p_command[index++] = command_code;
    p_command[index++] = command_code >> (8 * 1);

    uint16_t head_device_number = 0;
    for (uint8_t i = 0; i < 6; i++) {
        if (0 != p_read_item->head_device_number_string[i])
            head_device_number = TransformAsciiToHex(p_read_item->head_device_number_string[i]) + head_device_number * (((0x9c == (uint8_t)p_read_item->device_code) || (0x9d == (uint8_t)p_read_item->device_code) || (0xa0 == (uint8_t)p_read_item->device_code) || (0xb4 == (uint8_t)p_read_item->device_code)) ? 16 : 10);
        else
            break;
    }
    p_command[index++] = head_device_number;
    p_command[index++] = head_device_number >> (8 * 1);
    p_command[index++] = head_device_number >> (8 * 2);
    p_command[index++] = p_read_item->device_code;
    p_command[index++] = p_read_item->device_points_count;
    p_command[index++] = p_read_item->device_points_count >> 8;
    return index;
}

/**
 * @description: Melsec_3E_IQ_R Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t Melsec3eiqrGenerateCommand(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    uint16_t index = Melsec3eqlGenerateCommand(p_command, command_code, p_read_item) - 6;
    
    uint16_t head_device_number = 0;
    for (uint8_t i = 0; i < 6; i++) {
        if (0 != p_read_item->head_device_number_string[i])
            head_device_number = TransformAsciiToHex(p_read_item->head_device_number_string[i]) + head_device_number * (((0x9c == (uint8_t)p_read_item->device_code) || (0x9d == (uint8_t)p_read_item->device_code)|| (0xa0 == (uint8_t)p_read_item->device_code)|| (0xb4 == (uint8_t)p_read_item->device_code)) ? 16 : 10);
        else
            break;
    }
    p_command[index++] = head_device_number;
    p_command[index++] = head_device_number >> (8 * 1);
    p_command[index++] = head_device_number >> (8 * 2);
    p_command[index++] = head_device_number >> (8 * 3);
    p_command[index++] = p_read_item->device_code;
    p_command[index++] = p_read_item->device_code >> 8;
    p_command[index++] = p_read_item->device_points_count;
    p_command[index++] = p_read_item->device_points_count >> 8;
    return index;
}

/**
 * @description: Melsec_1C Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t Melsec1cGenerateCommand(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    p_read_item->monitoring_timer /= 10;
    uint16_t index = 0;
    uint8_t checksum = 0;

    p_command[index++] = MELSEC_ENQ;
    p_command[index++] = TransformHexToAscii(STATION_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(STATION_NUMBER);
    p_command[index++] = TransformHexToAscii(PC_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(PC_NUMBER);
    p_command[index++] = command_code >> 8;
    p_command[index++] = command_code;
    p_command[index++] = TransformHexToAscii(p_read_item->monitoring_timer);
    p_command[index++] = p_read_item->device_code;
    uint8_t head_device_number_string_length = 0;
    for (uint8_t i = 0; i < 6; i++) {
        if (0 == p_read_item->head_device_number_string[i])
            break;
        else
            head_device_number_string_length++;
    }
    p_command[index++] = (head_device_number_string_length - 4 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 4];
    p_command[index++] = (head_device_number_string_length - 3 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 3];
    p_command[index++] = (head_device_number_string_length - 2 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 2];
    p_command[index++] = (head_device_number_string_length - 1 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 1];
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count >> 4);
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count);
    checksum = GetCheckSum(p_command + 1, index - 1);
    p_command[index++] = TransformHexToAscii(checksum >> 4);
    p_command[index++] = TransformHexToAscii(checksum);
    return index;
}

/**
 * @description: Melsec_3C Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t Melsec3cGenerateCommand(uint8_t* p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    uint16_t index = 0;
    uint8_t checksum = 0;

    p_command[index++] = MELSEC_ENQ;
    p_command[index++] = FRAME_NUMBER >> 8;
    p_command[index++] = (uint8_t)FRAME_NUMBER;
    p_command[index++] = TransformHexToAscii(STATION_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(STATION_NUMBER);
    p_command[index++] = TransformHexToAscii(NETWORK_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(NETWORK_NUMBER);
    p_command[index++] = TransformHexToAscii(PC_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(PC_NUMBER);
    p_command[index++] = TransformHexToAscii(SELF_STATION_NUMBER >> 4);
    p_command[index++] = TransformHexToAscii(SELF_STATION_NUMBER);
    p_command[index++] = TransformHexToAscii(command_code >> (7 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (6 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (5 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (4 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (3 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (2 * 4));
    p_command[index++] = TransformHexToAscii(command_code >> (1 * 4));
    p_command[index++] = TransformHexToAscii(command_code);
    p_command[index++] = p_read_item->device_code >> 8;
    p_command[index++] = p_read_item->device_code;
    uint8_t head_device_number_string_length = 0;
    for (uint8_t i = 0; i < 6; i++) {
        if (0 == p_read_item->head_device_number_string[i])
            break;
        else
            head_device_number_string_length++;
    }
    p_command[index++] = (head_device_number_string_length - 6 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 6];
    p_command[index++] = (head_device_number_string_length - 5 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 5];
    p_command[index++] = (head_device_number_string_length - 4 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 4];
    p_command[index++] = (head_device_number_string_length - 3 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 3];
    p_command[index++] = (head_device_number_string_length - 2 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 2];
    p_command[index++] = (head_device_number_string_length - 1 < 0) ? 0x30 : p_read_item->head_device_number_string[head_device_number_string_length - 1];
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count >> (3 * 8));
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count >> (2 * 8));
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count >> (1 * 8));
    p_command[index++] = TransformHexToAscii(p_read_item->device_points_count);
    checksum = GetCheckSum(p_command + 1, index - 1);
    p_command[index++] = TransformHexToAscii(checksum >> 4);
    p_command[index++] = TransformHexToAscii(checksum);
    return index;
}

/**
 * @description: Melsec Cmd Genetare
 * @param p_command - command pointer
 * @param command_code - command code
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t MelsecGenerateCommand(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item)
{
    uint16_t (*GenerateMelsecCommandFunction)(uint8_t *p_command, uint32_t command_code, MelsecReadItem *p_read_item);
    
    switch (p_read_item->data_info.frame_type) {
    case MELSEC_1E_FRAME:
        GenerateMelsecCommandFunction = Melsec1eGenerateCommand;
        break;
    case MELSEC_3E_IQ_R_FRAME:
        GenerateMelsecCommandFunction = Melsec3eiqrGenerateCommand;
        break;
    case MELSEC_3E_Q_L_FRAME:
        GenerateMelsecCommandFunction = Melsec3eqlGenerateCommand;
        break;
    case MELSEC_1C_FRAME:
        GenerateMelsecCommandFunction = Melsec1cGenerateCommand;
        break;
    case MELSEC_3C_FRAME:
        GenerateMelsecCommandFunction = Melsec3cGenerateCommand;
        break;
    default:
        return 0;
    }
    return GenerateMelsecCommandFunction(p_command, command_code, p_read_item);
}

/**
 * @description: Melsec Data Info Init
 * @param p_read_item - read item pointer
 * @param p_data - control-data pointer
 * @return success : 0 error : -1
 */
int MelsecInitialDataInfo(MelsecReadItem *p_read_item, uint8_t *p_data)
{
    uint8_t check_sum = 0;
    BasicPlcDataInfo *p_base_data_info = &(p_read_item->data_info.base_data_info);

    int command_base_length = MelsecGetCommandBaseLength(p_read_item->data_info.frame_type);
    if (command_base_length < 0) {
        printf("%s Not supported device code!\n", __func__);
        return -1;
    }
    
    switch (p_read_item->data_info.command_type) {
    case READ_IN_BITS:
        p_base_data_info->command_length = command_base_length;
        p_base_data_info->p_command = PrivMalloc(command_base_length);
        p_base_data_info->data_size = p_read_item->device_points_count;
        p_base_data_info->p_data = p_data;
        break;
    case READ_IN_WORD:
        p_base_data_info->command_length = command_base_length;
        p_base_data_info->p_command = PrivMalloc(command_base_length);
        p_base_data_info->data_size = p_read_item->device_points_count * 2;
        p_base_data_info->p_data = p_data;
        break;
    case WRITE_IN_BITS:
        p_base_data_info->command_length = command_base_length + p_read_item->device_points_count;
        p_base_data_info->p_command = PrivMalloc(command_base_length + p_read_item->device_points_count);
        command_base_length -= (p_read_item->data_info.frame_type >= MELSEC_1C_FRAME) ? 2 : 0;
        memcpy(p_base_data_info->p_command + command_base_length, p_data, p_read_item->device_points_count);
        break;
    case WRITE_IN_WORD:
        p_base_data_info->command_length = command_base_length + p_read_item->device_points_count * 2;
        p_base_data_info->p_command = PrivMalloc(command_base_length + p_read_item->device_points_count * 2);
        command_base_length -= (p_read_item->data_info.frame_type >= MELSEC_1C_FRAME) ? 2 : 0;
        memcpy(p_base_data_info->p_command + command_base_length, p_data, p_read_item->device_points_count * 2);
        break;
    default:
        return -1;
    }

    uint32_t command_code = MelsecGetCommandCode(p_read_item->data_info.frame_type, p_read_item->data_info.command_type);
    MelsecGenerateCommand(p_base_data_info->p_command, command_code, p_read_item);

    return 0;
}

/**
 * @description: Melsec Data Transform from Receive Buffer To Control-Data
 * @param p_read_item - read item pointer
 * @param recv_buff - receive buff
 * @return success : 0 error : -1
 */
static int MelsecTransformRecvBuffToData(MelsecReadItem *p_read_item, uint8_t *recv_buff)
{
    MelsecDataInfo *p_melsec_data_info = &(p_read_item->data_info);
    MelsecFrameType frame_type = p_melsec_data_info->frame_type;
    MelsecCommandType command_type = p_melsec_data_info->command_type;
    uint8_t *p_data = p_melsec_data_info->base_data_info.p_data;

    uint16_t device_points_count = p_read_item->device_points_count;
    uint8_t is_ascii = ((MELSEC_1E_FRAME == frame_type) || (MELSEC_3E_Q_L_FRAME == frame_type) || (MELSEC_3E_IQ_R_FRAME == frame_type)) ? 0 : 1;
    uint16_t abnormal_code = 0;

    switch (frame_type) {
    case MELSEC_3E_IQ_R_FRAME:
    case MELSEC_3E_Q_L_FRAME:
        if (recv_buff[9] != 0 || recv_buff[10] != 0)
            abnormal_code = recv_buff[10] * 256 + recv_buff[9];
        else
            recv_buff += 11;
        break;
    case MELSEC_1E_FRAME:
        if (recv_buff[1] != 0)
            abnormal_code = recv_buff[2];
        else
            recv_buff += 2;
        break;
    case MELSEC_1C_FRAME:
        if (MELSEC_NAK == recv_buff[0])
            abnormal_code = recv_buff[5] * 256 + recv_buff[6];
        else
            recv_buff += 5;
        break;
    case MELSEC_3C_FRAME:
        if (MELSEC_NAK == recv_buff[0])
            abnormal_code = ((uint16_t)TransformAsciiToHex(recv_buff[11])) << 12 + ((uint16_t)TransformAsciiToHex(recv_buff[12])) << 8 +
            ((uint16_t)TransformAsciiToHex(recv_buff[13])) << 4 + ((uint16_t)TransformAsciiToHex(recv_buff[14]));
        else
            recv_buff += 11;
        break;
    default:
        return -1;
    }

    if (abnormal_code != 0) {
        printf("Data abnormal, abnormal code is %0x!", abnormal_code);
        return -1;
    }

    ControlPrintfList("DATA", recv_buff, (uint16_t)(device_points_count * (READ_IN_BITS == command_type ? 0.5 : 2) * (frame_type >= MELSEC_1C_FRAME ? 2 : 1) + 0.6));
    printf("Receive data is ");
    for (uint16_t i = 0; i < device_points_count; i++) {
        if (READ_IN_BITS == command_type) {
            if (!is_ascii) {
                p_data[i] = (recv_buff[i / 2] & (i % 2 == 0 ? 0x10 : 0x01)) || 0;
            } else {
                p_data[i] = TransformAsciiToHex(recv_buff[i]);
            }
            printf("0x%x", p_data[i]);
        } else if (READ_IN_WORD == command_type) {
            if (!is_ascii) {
                uint16_t recv_buff_index = 2 * (device_points_count - 1 - i);
                p_data[2 * i] = recv_buff[recv_buff_index + 1];
                p_data[2 * i + 1] = recv_buff[recv_buff_index];
            } else {
                uint16_t recv_buff_index = 4 * (device_points_count - 1 - i);
                p_data[2 * i] = TransformAsciiToHex(recv_buff[recv_buff_index]) * 16 + TransformAsciiToHex(recv_buff[recv_buff_index + 1]);
                p_data[2 * i + 1] = TransformAsciiToHex(recv_buff[recv_buff_index + 2]) * 16 + TransformAsciiToHex(recv_buff[recv_buff_index + 3]);
            }
            printf("0x%x 0x%x ", p_data[2 * i], p_data[2 * i + 1]);
        }
    }
    printf("\n");

    return 0;
}

#ifdef CONTROL_USING_SOCKET
/**
 * @description: Melsec Get Data From Socket
 * @param socket - socket
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int MelsecGetDataBySocket(int32_t socket, MelsecReadItem *p_read_item)
{
    uint8_t try_count = 0;
    int32_t write_error = 0;

    MelsecDataInfo *p_melsec_data_info = &(p_read_item->data_info);
    BasicPlcDataInfo *p_base_data_info = &(p_melsec_data_info->base_data_info);

    memset(recv_buff, 0, sizeof(recv_buff));

    while (try_count < 10) {
        ControlPrintfList("SEND", p_base_data_info->p_command, p_base_data_info->command_length);
        try_count++;

        write_error = socket_write(socket, p_base_data_info->p_command, p_base_data_info->command_length);
        if (write_error < 0) {
            printf("Write socket error, errno is %d!", errno);
        } else {
            PrivTaskDelay(20);

            int32_t recv_length = socket_read(socket, recv_buff, sizeof(recv_buff));
            if (recv_length < 0) {
                printf("Read socket error, errno is %d!", errno);
            } else {
                ControlPrintfList("RECV", recv_buff, recv_length);
                return MelsecTransformRecvBuffToData(p_read_item, recv_buff);
            }
        }

        if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            printf("Send plc command failed, errno is %d!", errno);
            continue;
        } else {
            return -1;
        }
    }
    return -2;
}
#endif

/**
 * @description: Melsec Get Data From Serial
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int MelsecGetDataBySerial(MelsecReadItem *p_read_item)
{
    uint32_t cmd_length,read_length = 0;
    memset(recv_buff, 0, sizeof(recv_buff));

    MelsecDataInfo *p_melsec_data_info = &(p_read_item->data_info);
    BasicPlcDataInfo *p_base_data_info = &(p_melsec_data_info->base_data_info); 
    MelsecCommandType melsec_command_type = p_melsec_data_info->command_type;//++
    MelsecFrameType melsec_frame_type= p_melsec_data_info->frame_type;//++

    uint16_t device_points_count = p_read_item->device_points_count;//++

    ControlPrintfList("SEND", p_base_data_info->p_command, p_base_data_info->command_length);
    SerialWrite(p_base_data_info->p_command, p_base_data_info->command_length);
    //++
   if (MELSEC_1C_FRAME == melsec_frame_type) {
      if (READ_IN_BITS == melsec_command_type ) {
          cmd_length = 9;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==1 ) {
          cmd_length = 12;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==2 ) {
          cmd_length = 16;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==4 ) {
          cmd_length = 24;
      } else {
        //MULTIPLE_COIL and MULTIPLE_REGISTER to do
        cmd_length = 0;
     }
   }else  if(MELSEC_3C_FRAME == melsec_frame_type){
    if (READ_IN_BITS == melsec_command_type ) {
          cmd_length = 15;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==1 ) {
          cmd_length = 18;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==2 ) {
          cmd_length = 22;
      } else if (READ_IN_WORD == melsec_command_type && device_points_count==4 ) {
          cmd_length = 30;
      } else {
        //MULTIPLE_COIL and MULTIPLE_REGISTER to do
        cmd_length = 0;
     }

   }
//++


    read_length = SerialRead(recv_buff, cmd_length);
    if (read_length) {
        ControlPrintfList("RECV", recv_buff, read_length);
        return MelsecTransformRecvBuffToData(p_read_item, recv_buff);
    }
}

/**
 * @description: Melsec Receive Plc Data Task
 * @param parameter - parameter pointer
 * @return
 */
void *ReceivePlcDataTask(void *parameter)
{
    int i = 0;
    uint8_t try_count = 0;
    uint16_t data_length = 0;
    uint8_t *melsec_data;
    uint16_t read_item_size = sizeof(MelsecReadItem);

    struct ControlProtocol *control_protocol = (struct ControlProtocol *)parameter;
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    MelsecReadItem *melsec_read_item = (MelsecReadItem *)control_protocol->recipe->read_item;
    melsec_data = control_protocol->recipe->protocol_data.data;
    data_length = control_protocol->recipe->protocol_data.data_length;

    memset(&plc_socket, 0, sizeof(BasicSocketPlc));
    memcpy(plc_socket.ip, control_protocol->recipe->socket_config.plc_ip, 4);
    plc_socket.port = control_protocol->recipe->socket_config.port;
    plc_socket.socket = -1;

    while (1) {
        for (i = 0; i < control_protocol->recipe->read_item_count; i ++) {

            if ((PROTOCOL_MELSEC_1C == control_protocol->protocol_type) || (PROTOCOL_MELSEC_3C == control_protocol->protocol_type)) {
                MelsecGetDataBySerial((MelsecReadItem *)melsec_read_item + i);
            } else {
#ifdef CONTROL_USING_SOCKET
                /*only connect socket when close socket or init*/
                while (ControlConnectSocket(&plc_socket) < 0) {
                    PrivTaskDelay(1000);
                }

                MelsecGetDataBySocket(plc_socket.socket, (MelsecReadItem *)melsec_read_item + i);
#endif
            }
        }

        /*read all variable item data, put them into circular_area*/
        if (i == control_protocol->recipe->read_item_count) {
            printf("%s get %d item %d length\n", __func__, i, data_length);
            CircularAreaAppWrite(circular_area, melsec_data, data_length, 0);
        }

        /*read data every single 'read_period' ms*/
        PrivTaskDelay(control_protocol->recipe->read_period);
    }
}

/**
 * @description: Melsec Protocol Open
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int MelsecOpen(struct ControlProtocol *control_protocol)
{
    ControlProtocolOpenDef(control_protocol);

    return 0;
}

/**
 * @description: Melsec Protocol Close
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int MelsecClose(struct ControlProtocol *control_protocol)
{
    if ((PROTOCOL_MELSEC_1C != control_protocol->protocol_type) && (PROTOCOL_MELSEC_3C != control_protocol->protocol_type)) {
#ifdef CONTROL_USING_SOCKET
        ControlDisconnectSocket(&plc_socket);
#endif
    }

    ControlProtocolCloseDef();

    return 0;
}

/**
 * @description: Melsec Protocol Read Data
 * @param control_protocol - control protocol pointer
 * @param buf - read data buffer
 * @param len - read data length
 * @return success : data length error : 0
 */
int MelsecRead(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    return CircularAreaAppRead(circular_area, buf, len);
}

static struct ControlDone melsec_protocol_done = 
{
    ._open = MelsecOpen,
    ._close = MelsecClose,
    ._read = MelsecRead,
    ._write = NULL,
    ._ioctl = NULL,
};

/**
 * @description: Melsec Protocol Cmd Generate
 * @param p_recipe - recipe pointer
 * @param protocol_format_info - protocol format info pointer
 * @return success : 0 error : -1
 */
int MelsecProtocolFormatCmd(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info)
{
    int ret = 0;
    static uint8_t last_item_size = 0;
    uint8_t *p_read_item_data = protocol_format_info->p_read_item_data + last_item_size;

    MelsecReadItem *melsec_read_item = (MelsecReadItem *)(p_recipe->read_item) + protocol_format_info->read_item_index;

    melsec_read_item->value_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_type")->valueint;
    strncpy(melsec_read_item->value_name, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_name")->valuestring, 20);
    melsec_read_item->data_info.command_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "command_type")->valueint;
    melsec_read_item->data_info.frame_type = p_recipe->protocol_type - PROTOCOL_MELSEC_1E;
    melsec_read_item->monitoring_timer = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "monitoring_timer")->valueint;
    melsec_read_item->device_code = MelsecGetDeviceCode(melsec_read_item->data_info.frame_type, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "device_code")->valuestring);
    strncpy(melsec_read_item->head_device_number_string, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "head_device_number_string")->valuestring, 6);
    melsec_read_item->device_points_count = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "device_points_count")->valueint;

    ret = MelsecInitialDataInfo(melsec_read_item, p_read_item_data);

    ControlPrintfList("CMD", melsec_read_item->data_info.base_data_info.p_command, melsec_read_item->data_info.base_data_info.command_length);
    protocol_format_info->last_item_size = GetValueTypeMemorySize(melsec_read_item->value_type);

    last_item_size += protocol_format_info->last_item_size;

    return ret;
}

/**
 * @description: Melsec Protocol Init
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int MelsecProtocolInit(struct ControlRecipe *p_recipe)
{   
    p_recipe->read_item = PrivMalloc(sizeof(MelsecReadItem) * p_recipe->read_item_count);
    if (NULL == p_recipe->read_item) {
        PrivFree(p_recipe->read_item);
        return -1;
    }

    memset(p_recipe->read_item, 0, sizeof(MelsecReadItem));

    p_recipe->ControlProtocolFormatCmd = MelsecProtocolFormatCmd;

    p_recipe->done = &melsec_protocol_done;

    return 0;
}
