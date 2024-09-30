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
 * @file fins.c
 * @brief plc protocol fins 
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-10-08
 */

#include <fins.h>

#define FINS_COMMAND_LENGTH 34

static BasicSocketPlc plc_socket = {0};

static uint8_t handshake_require_command[] = {0x46, 0x49, 0x4E, 0x53, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t handshake_respond_buff[24] = {0};
static uint8_t recv_buff[1024] = {0};

/**
 * @description: Fins Get Area Code
 * @param area_char - area char
 * @param type - fins data type
 * @return success : area_char error : 0
 */
static uint8_t FinsAreaCode(char area_char, FinsDataType type)
{
    uint8_t area_code = 0;
    if (area_char == 'C')
        area_code = (type == FINS_DATA_TYPE_BIT ? 0x30 : 0xB0);
    if (area_char == 'W')
        area_code = (type == FINS_DATA_TYPE_BIT ? 0x31 : 0xB1);
    if (area_char == 'H')
        area_code = (type == FINS_DATA_TYPE_BIT ? 0x32 : 0xB2);
    if (area_char == 'D')
        area_code = (type == FINS_DATA_TYPE_BIT ? 0x02 : 0x82);
    return area_code;
}

/**
 * @description: Fins Cmd Genetare
 * @param p_command - command pointer
 * @param plc_ip_4 - last plc ip
 * @param local_ip_4 - last local ip
 * @param p_read_item - p_read_item pointer
 * @return success : index error : 0
 */
static uint16_t FinsCommandGenerate(uint8_t *p_command, uint16_t plc_ip_4, uint16_t local_ip_4, FinsReadItem *p_read_item)
{
    uint8_t index = 0;
    uint16_t command_code = p_read_item->data_info.command_code;
    uint8_t area_char = p_read_item->area_char;
    uint16_t data_type = p_read_item->data_type;
    uint16_t start_address = p_read_item->start_address;
    uint8_t bit_address = p_read_item->bit_address;
    uint16_t data_length = p_read_item->data_length;

    p_command[index++] = (uint8_t)(FINS_HEADER_HEAD >> (3 * 8));
    p_command[index++] = (uint8_t)(FINS_HEADER_HEAD >> (2 * 8));
    p_command[index++] = (uint8_t)(FINS_HEADER_HEAD >> (1 * 8));
    p_command[index++] = (uint8_t)FINS_HEADER_HEAD;
    p_command[index++] = (uint8_t)FINS_HEADER_READ_COMMAND_LENGTH >> (3 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_READ_COMMAND_LENGTH >> (2 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_READ_COMMAND_LENGTH >> (1 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_READ_COMMAND_LENGTH;
    p_command[index++] = (uint32_t)FINS_HEADER_COMMAND >> (3 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_COMMAND >> (2 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_COMMAND >> (1 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_COMMAND;
    p_command[index++] = (uint32_t)FINS_HEADER_ERROR_CODE >> (3 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_ERROR_CODE >> (2 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_ERROR_CODE >> (1 * 8);
    p_command[index++] = (uint32_t)FINS_HEADER_ERROR_CODE;
    p_command[index++] = FINS_ICF;
    p_command[index++] = FINS_RSV;
    p_command[index++] = FINS_GCT;

    p_command[index++] = FINS_DNA;
    p_command[index++] = plc_ip_4;
    p_command[index++] = FINS_DA2;

    p_command[index++] = FINS_SNA;
    p_command[index++] = local_ip_4;
    p_command[index++] = FINS_SA2;

    p_command[index++] = FINS_SID;

    p_command[index++] = command_code >> 8;
    p_command[index++] = command_code;
    p_command[index++] = FinsAreaCode(area_char, data_type);
    p_command[index++] = start_address >> 8;
    p_command[index++] = start_address;
    p_command[index++] = bit_address;
    p_command[index++] = data_length >> 8;
    p_command[index++] = data_length;

    return index;
}

/**
 * @description: Fins Data Transform from Receive Buffer To Control-Data
 * @param p_read_item - read item pointer
 * @param recv_buff - receive buff
 * @return success : 0 error : -1
 */
static int FinsTransformRecvBuffToData(FinsReadItem *p_read_item, uint8_t *recv_buff)
{
    FinsDataInfo *p_fins_data_info = &(p_read_item->data_info);
    uint8_t error_code = recv_buff[15];
    if (error_code) {
        printf("Data abnormal, abnormal error code is 0x%x!\n", error_code);
        return -1;
    }
    recv_buff += 30;

    FinsCommandCode command_code = p_fins_data_info->command_code;
    if (FINS_COMMAND_CODE_READ == command_code) {

        uint16_t data_length = p_read_item->data_length;
        ControlPrintfList("DATA", recv_buff, data_length * (FINS_DATA_TYPE_BIT == p_read_item->data_type ? 1 : 2));
        printf("Receive data is ");

        if (FINS_DATA_TYPE_BIT == p_read_item->data_type) {
            memcpy(p_fins_data_info->base_data_info.p_data, recv_buff, data_length);

            printf("0x%x", p_fins_data_info->base_data_info.p_data[0]);
        } else {
            uint8_t *p_data = p_fins_data_info->base_data_info.p_data;

            for (uint16_t i = 0; i < data_length; i ++) {
                p_data[2 * i] = recv_buff[2 * (data_length - i - 1)];
                p_data[2 * i + 1] = recv_buff[2 * (data_length - i - 1) + 1];
                printf("0x%x 0x%x", p_data[2 * i], p_data[2 * i + 1]);
            }
        }

        printf("\nRead fins plc data success!\n");
    } else if (FINS_COMMAND_CODE_WRITE == command_code) {
        /*to do*/
        printf("Write fins plc cmd success!\n");
    }

    return 0;
}

#ifdef CONTROL_USING_SOCKET
/**
 * @description: Fins Protocol Handshake
 * @param socket - socket
 * @param local_ip_4 - last local ip
 * @return success : 0 error : -1 -2
 */
static int FinsHandshake(int32_t socket, uint16_t local_ip_4)
{
    handshake_require_command[18] = (uint8_t)(local_ip_4 >> 8);
    handshake_require_command[19] = (uint8_t)local_ip_4;
    uint8_t try_count = 0;

    while (try_count < 10) {
        ControlPrintfList("SEND", (uint8_t *)handshake_require_command, sizeof(handshake_require_command));
        int32_t write_error = socket_write(socket, handshake_require_command, sizeof(handshake_require_command));
        if (write_error < 0) {
            printf("Write socket error, errno is %d!", errno);
        } else {
            int32_t recv_length = socket_read(socket, (uint8_t *)handshake_respond_buff, sizeof(handshake_respond_buff));
            if (recv_length < 0) {
                printf("Read socket error, errno is %d!", errno);
            } else {
                ControlPrintfList("RECV", (uint8_t *)handshake_respond_buff, recv_length);

                /*check fins handshake respond*/
                uint8_t error_code = handshake_respond_buff[15];
                if (error_code == 0 || error_code == 0x21) {
                    return 0;
                } else {
                    printf("Fins handshake failed, errno is %05x!", handshake_respond_buff[15]);
                    return -1;
                }
            }
        }
        if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            printf("Send plc command failed, errno is %d!", errno);
            continue;
        } else {
            break;
        }
    }
    return -2;
}

/**
 * @description: Fins Get Data From Socket
 * @param socket - socket
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int FinsGetData(int32_t socket, FinsReadItem *p_read_item)
{
    uint8_t try_count = 0;
    int32_t write_error = 0;

    FinsDataInfo *p_fins_data_info = &(p_read_item->data_info);
    BasicPlcDataInfo *p_base_data_info = &(p_fins_data_info->base_data_info);

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
                return FinsTransformRecvBuffToData(p_read_item, recv_buff);
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
 * @description: Fins Data Info Init
 * @param p_read_item - read item pointer
 * @param plc_ip_4 - last plc ip
 * @param local_ip_4 - last local ip
 * @param p_data - control-data pointer
 * @return success : 0 error : -1
 */
static int FinsInitialDataInfo(FinsReadItem *p_read_item, uint16_t plc_ip_4, uint16_t local_ip_4, uint8_t *p_data)
{
    uint16_t data_length = p_read_item->data_length;

    BasicPlcDataInfo *p_base_data_info = &(p_read_item->data_info.base_data_info);
    switch (p_read_item->data_info.command_code) 
    {
    case FINS_COMMAND_CODE_READ:
        data_length *= (p_read_item->data_type == FINS_DATA_TYPE_BIT ? 1 : 2);
        p_base_data_info->command_length = FINS_COMMAND_LENGTH;
        p_base_data_info->p_command = PrivMalloc(FINS_COMMAND_LENGTH);
        p_base_data_info->data_size = data_length;
        p_base_data_info->p_data = p_data;
        break;
    case FINS_COMMAND_CODE_WRITE:
        //To Do
        break;
    default:
        return -1;
    }
    FinsCommandGenerate(p_base_data_info->p_command, plc_ip_4, local_ip_4, p_read_item);

    return 0;
}

/**
 * @description: Fins Receive Plc Data Task
 * @param parameter - parameter pointer
 * @return
 */
void *ReceivePlcDataTask(void *parameter)
{
    int i = 0;
    uint8_t try_count = 0;
    uint16_t data_length = 0;
    uint8_t *fins_data;
    uint16_t read_item_size = sizeof(FinsReadItem);

    struct ControlProtocol *control_protocol = (struct ControlProtocol *)parameter;
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    FinsReadItem *fins_read_item = (FinsReadItem *)control_protocol->recipe->read_item;
    fins_data = control_protocol->recipe->protocol_data.data;
    data_length = control_protocol->recipe->protocol_data.data_length;

    memset(&plc_socket, 0, sizeof(BasicSocketPlc));
    memcpy(plc_socket.ip, control_protocol->recipe->socket_config.plc_ip, 4);
    plc_socket.port = control_protocol->recipe->socket_config.port;
    plc_socket.socket = -1;
    plc_socket.secondary_connect_flag = 0;

    while (1) {
        for (i = 0; i < control_protocol->recipe->read_item_count; i ++) {
#ifdef CONTROL_USING_SOCKET
            /*only connect socket when close socket or init*/
            while (ControlConnectSocket(&plc_socket) < 0) {
                PrivTaskDelay(1000);
            }

            /*need to handshake after connect socket using FINS protocol*/
            if (0 == plc_socket.secondary_connect_flag) {
                if (FinsHandshake(plc_socket.socket, control_protocol->recipe->socket_config.local_ip[3]) < 0) {
                    plc_socket.secondary_connect_flag = 0;
                    ControlDisconnectSocket(&plc_socket);
                    continue;
                }
            } 

            plc_socket.secondary_connect_flag = 1;

            FinsGetData(plc_socket.socket, (FinsReadItem *)fins_read_item + i);
#endif
        }

        /*read all variable item data, put them into circular_area*/
        if (i == control_protocol->recipe->read_item_count) {
            printf("%s get %d item %d length\n", __func__, i, data_length);
            // for(int i = 0; i < data_length; i++){
            //     printf("%x\n",fins_data[i]);
            // }
            CircularAreaAppWrite(circular_area, fins_data, data_length, 0);
            printf("%s\n",fins_data);
        }

        /*read data every single 'read_period' ms*/
        PrivTaskDelay(control_protocol->recipe->read_period);
    }
}

/**
 * @description: Fins Protocol Open
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int FinsOpen(struct ControlProtocol *control_protocol)
{
    ControlProtocolOpenDef(control_protocol);

    return 0;
}

/**
 * @description: Fins Protocol Close
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int FinsClose(struct ControlProtocol *control_protocol)
{
#ifdef CONTROL_USING_SOCKET
    ControlDisconnectSocket(&plc_socket);
#endif
    
    ControlProtocolCloseDef();

    return 0;
}

/**
 * @description: Fins Protocol Read Data
 * @param control_protocol - control protocol pointer
 * @param buf - read data buffer
 * @param len - read data length
 * @return success : data length error : 0
 */
int FinsRead(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    return CircularAreaAppRead(circular_area, buf, len);
}

static struct ControlDone fins_protocol_done = 
{
    ._open = FinsOpen,
    ._close = FinsClose,
    ._read = FinsRead,
    ._write = NULL,
    ._ioctl = NULL,
};

/**
 * @description: Fins Protocol Cmd Generate
 * @param p_recipe - recipe pointer
 * @param protocol_format_info - protocol format info pointer
 * @return success : 0 error : -1
 */
int FinsProtocolFormatCmd(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info)
{
    int ret = 0;
    static uint8_t last_item_size = 0;
    uint8_t *p_read_item_data = protocol_format_info->p_read_item_data + last_item_size;

    FinsReadItem *fins_read_item = (FinsReadItem *)(p_recipe->read_item) + protocol_format_info->read_item_index;

    fins_read_item->value_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_type")->valueint;
    strncpy(fins_read_item->value_name, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_name")->valuestring, 20);
    strncpy(&fins_read_item->area_char, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "area_char")->valuestring, 1);
    fins_read_item->data_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "data_type")->valueint;
    fins_read_item->data_info.command_code = FINS_COMMAND_CODE_READ;
    fins_read_item->start_address = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "start_address")->valueint;
    fins_read_item->bit_address = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "bit_address")->valueint;
    fins_read_item->data_length = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "data_length")->valueint;

    ret = FinsInitialDataInfo(fins_read_item,
        p_recipe->socket_config.plc_ip[3],
        p_recipe->socket_config.local_ip[3],
        p_read_item_data);

    ControlPrintfList("CMD", fins_read_item->data_info.base_data_info.p_command, fins_read_item->data_info.base_data_info.command_length);
    protocol_format_info->last_item_size = GetValueTypeMemorySize(fins_read_item->value_type);

    last_item_size += protocol_format_info->last_item_size;

    return ret;
}

/**
 * @description: Fins Protocol Init
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int FinsProtocolInit(struct ControlRecipe *p_recipe)
{   
    p_recipe->read_item = PrivMalloc(sizeof(FinsReadItem) * p_recipe->read_item_count);
    if (NULL == p_recipe->read_item) {
        PrivFree(p_recipe->read_item);
        return -1;
    }

    memset(p_recipe->read_item, 0, sizeof(FinsReadItem));

    p_recipe->ControlProtocolFormatCmd = FinsProtocolFormatCmd;

    p_recipe->done = &fins_protocol_done;

    return 0;
}


