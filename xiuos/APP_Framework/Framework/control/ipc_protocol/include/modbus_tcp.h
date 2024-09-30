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
 * @file modbus_tcp.h
 * @brief support modbus_tcp function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.09.27
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include <control_def.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MODBUS_TCP_READ_CMD_LENGTH             0x0C
#define MODBUS_TCP_WRITE_SINGLE_CMD_LENGTH     0x0C

#define MODBUS_TCP_WRITE_MULTI_HEAD            0x07

typedef enum 
{
    READ_COIL_STATUS = 0x01,          //read coil cmd
    READ_INPUT_STATUS = 0x02,         //read input colr cmd
    READ_HOLDING_REGISTER = 0x03,     //read register info cmd
    READ_INPUT_REGISTER = 0x04,       //read input register cmd
    WRITE_SINGLE_COIL = 0x05,         //write coil cmd
    WRITE_SINGLE_REGISTER = 0x06,     //write single register cmd
    WRITE_MULTIPLE_COIL = 0x0F,       //write multi coil cmd
    WRITE_MULTIPLE_REGISTER = 0x10    //write multi register cmd
}ModbusTcpFunctionCode;

typedef struct 
{
    BasicPlcDataInfo base_data_info;
    ModbusTcpFunctionCode function_code;
}ModbusTcpDataInfo;

typedef struct
{
    ModbusTcpDataInfo data_info;

    UniformValueType value_type;
    char value_name[20];
    
    uint16_t start_address;
    uint16_t quantity;
}ModbusTcpReadItem;

#ifdef __cplusplus
}
#endif

#endif
