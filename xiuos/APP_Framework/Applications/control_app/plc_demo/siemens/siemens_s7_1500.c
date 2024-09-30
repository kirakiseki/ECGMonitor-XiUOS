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
 * @file siemens_s7_1500.c
 * @brief PLC SIEMENS S7-1500 app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.3.27
 */

#include <control.h>
void ControlS71500Test(void)
{
    int i = 0;
    uint16_t read_data_length = 0;
    uint8_t read_data[1024] = {0};
    ControlProtocolType s7_protocol = ControlProtocolFind();
    if (NULL == s7_protocol) {
        printf("%s get s7 protocol %p failed\n", __func__, s7_protocol);
        return;
    }
    printf("%s get s7 protocol %p successfull\n", __func__, s7_protocol);
    if (CONTROL_REGISTERED == s7_protocol->protocol_status) {
        ControlProtocolOpen(s7_protocol);
        for (;;) {
            read_data_length = ControlProtocolRead(s7_protocol, read_data, sizeof(read_data));
            printf("%s read [%d] s7 data %d using receipe file\n", __func__, i, read_data_length);
            i++;
            memset(read_data, 0, sizeof(read_data));
            PrivTaskDelay(10000);
        }
    } 
}

PRIV_SHELL_CMD_FUNCTION(ControlS71500Test, Siemens Plc S7_1512 Demo, PRIV_SHELL_CMD_MAIN_ATTR);