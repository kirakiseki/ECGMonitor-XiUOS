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
 * @file kv_8000.c
 * @brief PLC KEYENCE KV-8000 app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.4.12
 */

#include <control.h>
void ControlKV8000Test(void)
{
    int i, j = 0;
    int read_data_length = 0;
    uint8_t read_data[128] = {0};
    ControlProtocolType modbus_tcp_protocol = ControlProtocolFind();
    if (NULL == modbus_tcp_protocol) {
        printf("%s get modbus tcp protocol %p failed\n", __func__, modbus_tcp_protocol);
        return;
    }
    printf("%s get modbus tcp protocol %p successfull\n", __func__, modbus_tcp_protocol);

    if (CONTROL_REGISTERED == modbus_tcp_protocol->protocol_status) {
        ControlProtocolOpen(modbus_tcp_protocol);
        for (;;) {
            read_data_length = ControlProtocolRead(modbus_tcp_protocol, read_data, sizeof(read_data));
            printf("%s read [%d] modbus tcp data %d using receipe file\n", __func__, i, read_data_length);
            if (read_data_length) {
                for (j = 0; j < read_data_length; j ++) {
                    printf("j %d data 0x%x\n", j, read_data[j]);
                }
            }
            i++;
            memset(read_data, 0, sizeof(read_data));          
            PrivTaskDelay(10000);
        }
        //ControlProtocolClose(modbus_tcp_protocol);
    }
}
PRIV_SHELL_CMD_FUNCTION(ControlKV8000Test, KEYENCE Plc KV_8000 Demo, PRIV_SHELL_CMD_MAIN_ATTR);


