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
 * @file modbus_uart.c
 * @brief support modbus_uart function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.12.29
 */

#include <modbus_uart.h>

/*using cirtular area to receive write-data*/
#define CA_DATA_LENGTH 512
struct CircularAreaApp *g_write_data;

static BasicSocketPlc plc_socket = {0};
static uint8_t recv_buff[1024] = {0};

/**
 * @description: CRC16 check
 * @param data data buffer
 * @param length data length
 * @return check code
 */
static uint16_t ModbusUartCrc16(uint8_t *data, uint32_t length)
{
    int j;
    uint16_t reg_crc = 0xFFFF;

    while (length--) {
        reg_crc ^= *data++;
        for (j = 0;j < 8;j ++) {
            if(reg_crc & 0x01)
                reg_crc = reg_crc >> 1 ^ 0xA001;
            else
                reg_crc = reg_crc >> 1;
        }
    }
    printf(" crc = [0x%x]\n", reg_crc);
    return reg_crc;
}

/**
 * @description: Modbus Uart Data Transform from Receive Buffer To Control-Data
 * @param p_read_item - read item pointer
 * @param recv_buff - receive buff
 * @return success : 0 error : -1
 */
static int ModbusUartTransformRecvBuffToData(ModbusUartReadItem *p_read_item, uint8_t *recv_buff)
{
    uint8_t head_length = 3;
    uint8_t *data_buffer;
    ModbusUartDataInfo *p_modbus_uart_data_info = &(p_read_item->data_info);
    uint16_t quantity = p_read_item->quantity;

    ModbusUartFunctionCode function_code = p_modbus_uart_data_info->function_code;
    uint8_t *p_data = p_modbus_uart_data_info->base_data_info.p_data;

    uint8_t bytes_count = recv_buff[2];

    if ((WRITE_SINGLE_COIL == function_code) || (WRITE_SINGLE_REGISTER == function_code)) {
        head_length = 4;
        if (p_modbus_uart_data_info->base_data_info.command_ready) {
            p_modbus_uart_data_info->base_data_info.command_ready = 0;
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

    return 0;
}

/**
 * @description: Modbus Uart Get Data From Serial
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int ModbusUartGetDataBySerial(ModbusUartReadItem *p_read_item)
{
    uint32_t cmd_length, read_length = 0;
    memset(recv_buff, 0, sizeof(recv_buff));

    ModbusUartDataInfo *p_modbus_uart_data_info = &(p_read_item->data_info);
    BasicPlcDataInfo *p_base_data_info = &(p_modbus_uart_data_info->base_data_info);   
    ModbusUartFunctionCode function_code = p_modbus_uart_data_info->function_code;
    uint16_t quantity = p_read_item->quantity;//++

    ControlPrintfList("SEND", p_base_data_info->p_command, p_base_data_info->command_length);
    SerialWrite(p_base_data_info->p_command, p_base_data_info->command_length);

    if (READ_COIL_STATUS == function_code || READ_INPUT_STATUS == function_code) {
        cmd_length = 6;
    } else if ((READ_HOLDING_REGISTER == function_code || READ_INPUT_REGISTER == function_code) && quantity == 1 ) {
        cmd_length = 7;
    } else if ((READ_HOLDING_REGISTER == function_code || READ_INPUT_REGISTER == function_code) && quantity == 2 ) {
        cmd_length = 9;
    } else if ((READ_HOLDING_REGISTER == function_code || READ_INPUT_REGISTER == function_code) && quantity == 4 ) {
        cmd_length = 13;    
    } else if (WRITE_SINGLE_COIL == function_code || WRITE_SINGLE_REGISTER == function_code) {
        cmd_length = 8;
    } else {
        //MULTIPLE_COIL and MULTIPLE_REGISTER to do
        cmd_length = 0;
    }

    read_length = SerialRead(recv_buff, cmd_length);
    if (read_length) {
        ControlPrintfList("RECV", recv_buff, read_length);
        return ModbusUartTransformRecvBuffToData(p_read_item, recv_buff);
    }
}

/**
 * @description: Modbus Uart Data Info Init
 * @param p_read_item - read item pointer
 * @param station - uart station number
 * @param p_data - control-data pointer
 * @return success : 0 error : -1 -2
 */
static int ModbusUartInitialDataInfo(ModbusUartReadItem *p_read_item, uint8_t station, uint8_t *p_data)
{
    uint16_t command_crc = 0;
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
        p_base_data_info->command_length = MODBUS_UART_READ_CMD_LENGTH;
        p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);
        p_base_data_info->p_data = p_data;
        p_base_data_info->command_ready = 1;
        break;
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        if (p_data == NULL) {
            return -1;
        } else {
            p_base_data_info->command_length = MODBUS_UART_WRITE_CMD_LENGTH;
            p_base_data_info->p_command = PrivMalloc(p_base_data_info->command_length);
            p_base_data_info->p_data = p_data;
            p_base_data_info->data_size = 2;
            p_base_data_info->command_ready = 0;
        }
        break;
    case WRITE_MULTIPLE_COIL:
    case WRITE_MULTIPLE_REGISTER:
        //to do
        printf("%s unsupported function code %d\n", __func__, function_code);
        return -1;
    default:
        return -2;
    }

    memset(p_base_data_info->p_command, 0, p_base_data_info->command_length);

    p_base_data_info->p_command[0] = station;
    p_base_data_info->p_command[1] = function_code;
    p_base_data_info->p_command[2] = start_address >> 8;
    p_base_data_info->p_command[3] = start_address;
    if ((function_code != WRITE_SINGLE_COIL) && (function_code != WRITE_SINGLE_REGISTER)) {
        p_base_data_info->p_command[4] = quantity >> 8;
        p_base_data_info->p_command[5] = quantity;
        command_crc = ModbusUartCrc16(p_base_data_info->p_command, 6);
        p_base_data_info->p_command[6] = command_crc & 0xFF;
        p_base_data_info->p_command[7] = (command_crc >> 8) & 0xFF;
    }

    return 0;
}

/**
 * @description: Modbus Uart Format write data from "g_write_data"
 * @param p_read_item - read item pointer
 * @return success : 0 error : -1 -2
 */
static int ModbusUartForamatWriteData(ModbusUartReadItem *p_read_item)
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
        command_index = 4;
        break;
    case WRITE_MULTIPLE_COIL:
    case WRITE_MULTIPLE_REGISTER:
        printf("%s unsupported function code %d\n", __func__, function_code);
        return -1;
    default:
        return -2;
    }

    for (i = 0; i < write_data_length; i ++) {
        p_base_data_info->p_command[command_index + i] = write_data_buffer[i];
    }

    p_base_data_info->command_ready = 1;

    return write_data_length;
}

/**
 * @description: Modbus Uart Receive Plc Data Task
 * @param parameter - parameter pointer
 * @return
 */
void *ReceivePlcDataTask(void *parameter)
{
    int i = 0;
    uint8_t try_count = 0;
    uint16_t data_length = 0;
    uint8_t *modbus_uart_data;
    uint16_t read_item_size = sizeof(ModbusUartReadItem);

    struct ControlProtocol *control_protocol = (struct ControlProtocol *)parameter;
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    ModbusUartReadItem *modbus_uart_read_item = (ModbusUartReadItem *)control_protocol->recipe->read_item;
    modbus_uart_data = control_protocol->recipe->protocol_data.data;
    data_length = control_protocol->recipe->protocol_data.data_length;

    while (1) {
        for (i = 0; i < control_protocol->recipe->read_item_count; i ++) {

            ModbusUartForamatWriteData((ModbusUartReadItem *)modbus_uart_read_item + i);

            ModbusUartGetDataBySerial((ModbusUartReadItem *)modbus_uart_read_item + i);
        }

        /*read all variable item data, put them into circular_area*/
        if (i == control_protocol->recipe->read_item_count) {
            printf("%s get %d item %d length modbus_uart_data %p\n", __func__, i, data_length, modbus_uart_data);
            CircularAreaAppWrite(circular_area, modbus_uart_data, data_length, 0);
        }

        /*read data every single 'read_period' ms*/
        PrivTaskDelay(control_protocol->recipe->read_period);
    }
}

/**
 * @description: Modbus Uart Protocol Open
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int ModbusUartOpen(struct ControlProtocol *control_protocol)
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
 * @description: Modbus Uart Protocol Close
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int ModbusUartClose(struct ControlProtocol *control_protocol)
{
    CircularAreaAppRelease(g_write_data);
    
    ControlProtocolCloseDef();

    return 0;
}

/**
 * @description: Modbus Uart Protocol Read Data
 * @param control_protocol - control protocol pointer
 * @param buf - read data buffer
 * @param len - read data length
 * @return success : data length error : 0
 */
int ModbusUartRead(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    return CircularAreaAppRead(circular_area, buf, len);
}

/**
 * @description: Modbus Uart Protocol Write Data
 * @param control_protocol - control protocol pointer
 * @param buf - write data buffer
 * @param len - write data length
 * @return success : data length error : 0
 */
int ModbusUartWrite(struct ControlProtocol *control_protocol, const void *buf, size_t len)
{
    CircularAreaAppWrite(g_write_data, (uint8_t *)buf, len, 0);

    return 0;
}

/**
 * @description: Modbus Uart Protocol Ioctl
 * @param control_protocol - control protocol pointer
 * @param cmd - ioctl cmd
 * @param args - ioctl args
 * @return success : data length error : 0
 */
int ModbusUartIoctl(struct ControlProtocol *control_protocol, int cmd, void *args)
{
    //to do
    return 0;
}

static struct ControlDone modbusuart_protocol_done = 
{
    ._open = ModbusUartOpen,
    ._close = ModbusUartClose,
    ._read = ModbusUartRead,
    ._write = ModbusUartWrite,
    ._ioctl = ModbusUartIoctl,
};

/**
 * @description: Modbus Uart Protocol Cmd Generate
 * @param p_recipe - recipe pointer
 * @param protocol_format_info - protocol format info pointer
 * @return success : 0 error : -1
 */
int ModbusUartProtocolFormatCmd(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info)
{
    int ret = 0;
    static uint8_t last_item_size = 0;
    uint8_t *p_read_item_data = protocol_format_info->p_read_item_data + last_item_size;

    ModbusUartReadItem *modbusuart_read_item = (ModbusUartReadItem *)(p_recipe->read_item) + protocol_format_info->read_item_index;

    modbusuart_read_item->value_type = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_type")->valueint;
    strncpy(modbusuart_read_item->value_name, cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "value_name")->valuestring, 20);
    modbusuart_read_item->data_info.function_code = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "function_code")->valueint;
    modbusuart_read_item->start_address = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "start_address")->valueint;
    modbusuart_read_item->quantity = cJSON_GetObjectItem(protocol_format_info->read_single_item_json, "quantity")->valueint;

    ret = ModbusUartInitialDataInfo(modbusuart_read_item,
        p_recipe->serial_config.station,
        p_read_item_data);

    ControlPrintfList("CMD", modbusuart_read_item->data_info.base_data_info.p_command, modbusuart_read_item->data_info.base_data_info.command_length);
    protocol_format_info->last_item_size = GetValueTypeMemorySize(modbusuart_read_item->value_type);

    last_item_size += protocol_format_info->last_item_size;

    return ret;

}

/**
 * @description: Modbus Uart Protocol Init
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int ModbusUartProtocolInit(struct ControlRecipe *p_recipe)
{   
    p_recipe->read_item = PrivMalloc(sizeof(ModbusUartReadItem) * p_recipe->read_item_count);
    if (NULL == p_recipe->read_item) {
        PrivFree(p_recipe->read_item);
        return -1;
    }

    memset(p_recipe->read_item, 0, sizeof(ModbusUartReadItem));

    p_recipe->ControlProtocolFormatCmd = ModbusUartProtocolFormatCmd;

    p_recipe->done = &modbusuart_protocol_done;

    return 0;
}
