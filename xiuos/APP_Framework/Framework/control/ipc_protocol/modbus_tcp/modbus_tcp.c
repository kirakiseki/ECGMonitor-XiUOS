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
 * @file modbus_tcp.c
 * @brief support modbus_tcp function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.12.09
 */

#include <modbus_tcp.h>

/*using cirtular area to receive write-data*/
#define CA_DATA_LENGTH 512
struct CircularAreaApp *g_write_data;

static BasicSocketPlc plc_socket = {0};
static uint8_t recv_buff[1024] = {0};

/**
 * @description: Modbus Tcp Data Transform from Receive Buffer To Control-Data
 * @param p_read_item - read item pointer
 * @param recv_buff - receive buff
 * @return success : 0 error : -1
 */
static void ModbusTcpTransformRecvBuffToData(ModbusTcpReadItem *p_read_item, uint8_t *recv_buff)
{
    uint8_t head_length = 9;
    uint8_t *data_buffer;
    ModbusTcpDataInfo *p_modbus_tcp_data_info = &(p_read_item->data_info);
    uint16_t quantity = p_read_item->quantity;

    ModbusTcpFunctionCode function_code = p_modbus_tcp_data_info->function_code;
    uint8_t *p_data = p_modbus_tcp_data_info->base_data_info.p_data;

    uint8_t bytes_count = recv_buff[8];

    if ((WRITE_SINGLE_COIL == function_code) || (WRITE_SINGLE_REGISTER == function_code) || 
        (WRITE_MULTIPLE_COIL == function_code) || (WRITE_MULTIPLE_REGISTER == function_code)) {
        head_length = 10;
        if (p_modbus_tcp_data_info->base_data_info.command_ready) {
            p_modbus_tcp_data_info->base_data_info.command_ready = 0;
        }
    }

    data_buffer = recv_buff + head_length;//remove head data

    if (READ_COIL_STATUS == function_code || READ_INPUT_STATUS == function_code) {
        printf("Receive data is ");
        for (int i = 0;i < bytes_count;i ++) {
            for (int j = 0;j < 8;j ++) {
                if ((i * 8 + j) < p_read_item->quantity) {
                    *(uint8_t *)(p_data + i * 8 + j) = ((data_buffer[i] >> j) & 0x01) ? 1 : 0;
                    printf("0x%x", *(uint8_t *)(p_data + i * 8 + j));
                }
            }
        }
    } else if (READ_HOLDING_REGISTER == function_code || READ_INPUT_REGISTER == function_code) {
        printf("Receive data is ");
        for (uint16_t i = 0; i < quantity; i ++) {
            ((int16_t *)p_data)[i] = ((int16_t *)data_buffer)[quantity - i - 1];
            printf("0x%x 0x%x ", p_data[2 * i], p_data[2 * i + 1]);
        }
    }
    printf("\n");
}

#ifdef CONTROL_USING_SOCKET
/**
 * @description: Modbus Tcp Get Data From Socket
 * @param socket - socket
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int ModbusTcpGetData(int32_t socket, ModbusTcpReadItem *p_read_item)
{
    uint8_t try_count = 0;
    int32_t write_error = 0;

    ModbusTcpDataInfo *p_modbus_tcp_data_info = &(p_read_item->data_info);
    BasicPlcDataInfo *p_base_data_info = &(p_modbus_tcp_data_info->base_data_info);

    if (!p_base_data_info->command_ready) {
        //command not ready, just return
        return 1;
    }

    memset(recv_buff, 0, sizeof(recv_buff));

    while (try_count < 10) {
        ControlPrintfList("SEND", p_base_data_info->p_command, p_base_data_info->command_length);
        try_count++;

        write_error = socket_write(socket, p_base_data_info->p_command, p_base_data_info->command_length);
        if (write_error < 0) {
            printf("Write socket error, errno is %d!\n", errno);
        } else {
            PrivTaskDelay(20);

            int32_t recv_length = socket_read(socket, recv_buff, sizeof(recv_buff));
            if (recv_length < 0) {
                printf("Read socket error, errno is %d! read again\n", errno);
                memset(recv_buff, 0, sizeof(recv_buff));
                recv_length = socket_read(socket, recv_buff, sizeof(recv_buff));
                if (recv_length > 0) {
                    ControlPrintfList("RECV", recv_buff, recv_length);
                    ModbusTcpTransformRecvBuffToData(p_read_item, recv_buff);
                    return 0;
                }
            } else {
                ControlPrintfList("RECV", recv_buff, recv_length);
                ModbusTcpTransformRecvBuffToData(p_read_item, recv_buff);

                return 0;
            }
        }

        if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            printf("Send command failed, errno is %d!\n", errno);
            continue;
        } else {
            return -1;
        }
    }
    return -2;
}
#endif

/**
 * @description: Modbus Tcp Data Info Init
 * @param p_read_item - read item pointer
 * @param index - read item index
 * @param p_data - control-data pointer
 * @return success : 0 error : -1
 */
static int ModbusTcpInitialDataInfo(ModbusTcpReadItem *p_read_item, uint16_t index, uint8_t *p_data)
{
    uint16_t command_index = 0;
    uint8_t function_code = p_read_item->data_info.function_code;
    uint16_t start_address = p_read_item->start_address;
    uint16_t quantity = p_read_item->quantity;

    BasicPlcDataInfo *p_base_data_info = &(p_read_item->data_info.base_data_info);

    switch (function_code) 
    {
    case READ_COIL_STATUS:
    case READ_INPUT_STATUS:
    case READ_HOLDING_REGISTER:
    case READ_INPUT_REGISTER:
        p_base_data_info->command_length = MODBUS_TCP_READ_CMD_LENGTH;
        p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);
        p_base_data_info->p_data = p_data;
        p_base_data_info->command_ready = 1;
        break;
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        if (p_data == NULL) {
            return -1;
        } else {
            p_base_data_info->command_length = MODBUS_TCP_WRITE_SINGLE_CMD_LENGTH;
            p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);
            p_base_data_info->p_data = p_data;
            p_base_data_info->data_size = 2;
            p_base_data_info->command_ready = 0;
        }
        break;
    case WRITE_MULTIPLE_COIL:
        if (p_data == NULL) {
            return -1;
        } else {
            //"quantity" define how many coil need to be written,"n_byte" define the bytes of write-data(counted by bit)
            uint16_t n_byte = (quantity - 1) / 8 + 1;
            p_base_data_info->command_length = n_byte + MODBUS_TCP_WRITE_MULTI_HEAD + 6;
            p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);

            //13th command define the bytes of write-data
            p_base_data_info->p_command[12] = ((quantity - 1) / 8 + 1);
            p_base_data_info->p_data = p_data;
            p_base_data_info->data_size = n_byte;
            p_base_data_info->command_ready = 0;
        }
        break;
    case WRITE_MULTIPLE_REGISTER:
        if (p_data == NULL) {
            return -1;
        } else {
            //"quantity" define how many register need to be written
            p_base_data_info->command_length = quantity * 2 + MODBUS_TCP_WRITE_MULTI_HEAD + 6;
            p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);

            //13th command define the bytes of write-data
            p_base_data_info->p_command[12] = 2 * quantity;
            p_base_data_info->p_data = p_data;
            p_base_data_info->data_size = quantity * 2;
            p_base_data_info->command_ready = 0;
        }
        break;
    default:
        return -2;
        break;
    }

    memset(p_base_data_info->p_command, 0, p_base_data_info->command_length);

    p_base_data_info->p_command[0] = index >> 8;
    p_base_data_info->p_command[1] = index;
    p_base_data_info->p_command[2] = 0x00;
    p_base_data_info->p_command[3] = 0x00;
    p_base_data_info->p_command[4] = 0x00;

    if (function_code < WRITE_MULTIPLE_COIL) {
        p_base_data_info->p_command[5] = 0x06;
    } else {
        p_base_data_info->p_command[5] = 0x09;
    }

    p_base_data_info->p_command[6] = control_protocol->recipe->device_id;
    p_base_data_info->p_command[7] = function_code;
    p_base_data_info->p_command[8] = start_address >> 8;
    p_base_data_info->p_command[9] = start_address;

    if ((function_code != WRITE_SINGLE_COIL) && (function_code != WRITE_SINGLE_REGISTER)) {
        p_base_data_info->p_command[10] = quantity >> 8;
        p_base_data_info->p_command[11] = quantity;
    }
    return 0;
}

/**
 * @description: Modbus Tcp Format write data from "g_write_data"
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1
 */
static int ModbusTcpForamatWriteData(ModbusTcpReadItem *p_read_item)
{
    int i = 0;
    uint16_t command_index = 0;
    int write_data_length = 0;
    uint8_t write_data_buffer[32] = {0};

    BasicPlcDataInfo *p_base_data_info = &(p_read_item->data_info.base_data_info);
    uint8_t *p_command = p_base_data_info->p_command;
    uint8_t function_code = p_read_item->data_info.function_code;

    if (function_code < WRITE_SINGLE_COIL) {
        return 0;
    }

    write_data_length = CircularAreaAppRead(g_write_data, write_data_buffer, p_base_data_info->data_size);
    if (p_base_data_info->data_size != write_data_length) {
        //printf("%s get write data %d [should be %d]failed!\n", __func__, write_data_length, p_base_data_info->data_size);
        return 0;
    }

    switch (function_code) 
    {
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        command_index = 10;
        break;
    case WRITE_MULTIPLE_COIL:
    case WRITE_MULTIPLE_REGISTER:
        command_index = 13;
        break;
    default:
        return -2;
        break;
    }

    for (i = 0; i < write_data_length; i ++) {
        p_base_data_info->p_command[command_index + i] = write_data_buffer[i];
    }

    p_base_data_info->command_ready = 1;

    return write_data_length;
}

/**
 * @description: Modbus Tcp Receive Plc Data Task
 * @param parameter - parameter pointer
 * @return
 */
void *ReceivePlcDataTask(void *parameter)
{
    int i = 0;
    uint8_t try_count = 0;
    uint16_t data_length = 0;
    uint8_t *modbus_tcp_data;
    uint16_t read_item_size = sizeof(ModbusTcpReadItem);

    struct ControlProtocol *control_protocol = (struct ControlProtocol *)parameter;
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    ModbusTcpReadItem *modbus_tcp_read_item = (ModbusTcpReadItem *)control_protocol->recipe->read_item;
    modbus_tcp_data = control_protocol->recipe->protocol_data.data;
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

            ModbusTcpForamatWriteData((ModbusTcpReadItem *)modbus_tcp_read_item + i);

            ModbusTcpGetData(plc_socket.socket, (ModbusTcpReadItem *)modbus_tcp_read_item + i);
#endif
        }

        /*read all variable item data, put them into circular_area*/
        if (i == control_protocol->recipe->read_item_count) {
            printf("%s get %d item %d length modbus_tcp_data %p\n", __func__, i, data_length, modbus_tcp_data);
            CircularAreaAppWrite(circular_area, modbus_tcp_data, data_length, 0);
        }

        /*read data every single 'read_period' ms*/
        PrivTaskDelay(control_protocol->recipe->read_period);
    }
}

/**
 * @description: Modbus Tcp Protocol Open
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int ModbusTcpOpen(struct ControlProtocol *control_protocol)
{
    ControlProtocolOpenDef(control_protocol);

    g_write_data = CircularAreaAppInit(CA_DATA_LENGTH);
    if (NULL == g_write_data) {
        printf("%s CircularAreaInit error\n", __func__);
        return -1;
    }

    return 0;
}

/**
 * @description: Modbus Tcp Protocol Close
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int ModbusTcpClose(struct ControlProtocol *control_protocol)
{
    CircularAreaAppRelease(g_write_data);
    
#ifdef CONTROL_USING_SOCKET
    ControlDisconnectSocket(&plc_socket);
#endif
    
    ControlProtocolCloseDef();

    return 0;
}

/**
 * @description: Modbus Tcp Protocol Read Data
 * @param control_protocol - control protocol pointer
 * @param buf - read data buffer
 * @param len - read data length
 * @return success : data length error : 0
 */
int ModbusTcpRead(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    return CircularAreaAppRead(circular_area, buf, len);
}

/**
 * @description: Modbus Tcp Protocol Write Data
 * @param control_protocol - control protocol pointer
 * @param buf - write data buffer
 * @param len - write data length
 * @return success : data length error : 0
 */
int ModbusTcpWrite(struct ControlProtocol *control_protocol, const void *buf, size_t len)
{
    CircularAreaAppWrite(g_write_data, (uint8_t *)buf, len, 0);

    return 0;
}

/**
 * @description: Modbus Tcp Protocol Ioctl
 * @param control_protocol - control protocol pointer
 * @param cmd - ioctl cmd
 * @param args - ioctl args
 * @return success : data length error : 0
 */
int ModbusTcpIoctl(struct ControlProtocol *control_protocol, int cmd, void *args)
{
    //to do
    return 0;
}

static struct ControlDone modbustcp_protocol_done = 
{
    ._open = ModbusTcpOpen,
    ._close = ModbusTcpClose,
    ._read = ModbusTcpRead,
    ._write = ModbusTcpWrite,
    ._ioctl = ModbusTcpIoctl,
};

/**
 * @description: Modbus TCP Protocol Cmd Generate
 * @param p_recipe - recipe pointer
 * @param protocol_format_info - protocol format info pointer
 * @return success : 0 error : -1
 */
int ModbusTcpProtocolFormatCmd(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info)
{
    int ret = 0;
    static uint8_t last_item_size = 0;
    uint8_t *p_read_item_data = protocol_format_info->p_read_item_data + last_item_size;

    ModbusTcpReadItem *modbustcp_read_item = (ModbusTcpReadItem *)(p_recipe->read_item) + protocol_format_info->read_item_index;

    modbustcp_read_item->value_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_type")->valueint;
    strncpy(modbustcp_read_item->value_name, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_name")->valuestring, 20);
    modbustcp_read_item->data_info.function_code = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "function_code")->valueint;
    modbustcp_read_item->start_address = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "start_address")->valueint;
    modbustcp_read_item->quantity = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "quantity")->valueint;

    ret = ModbusTcpInitialDataInfo(modbustcp_read_item,
        protocol_format_info->read_item_index,
        p_read_item_data);

    ControlPrintfList("CMD", modbustcp_read_item->data_info.base_data_info.p_command, modbustcp_read_item->data_info.base_data_info.command_length);
    protocol_format_info->last_item_size = GetValueTypeMemorySize(modbustcp_read_item->value_type);

    last_item_size += protocol_format_info->last_item_size;

    return ret;

}

/**
 * @description: Modbus TCP Protocol Init
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int ModbusTcpProtocolInit(struct ControlRecipe *p_recipe)
{   
    p_recipe->read_item = PrivMalloc(sizeof(ModbusTcpReadItem) * p_recipe->read_item_count);
    if (NULL == p_recipe->read_item) {
        PrivFree(p_recipe->read_item);
        return -1;
    }

    memset(p_recipe->read_item, 0, sizeof(ModbusTcpReadItem));

    p_recipe->ControlProtocolFormatCmd = ModbusTcpProtocolFormatCmd;

    p_recipe->done = &modbustcp_protocol_done;

    return 0;
}

