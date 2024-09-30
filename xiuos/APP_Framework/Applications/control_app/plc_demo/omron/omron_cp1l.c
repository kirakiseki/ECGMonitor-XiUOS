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
 * @file omron_.cp1l.c
 * @brief PLC OMRON CP1L app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.2.10
 */
#include <control.h>

void ControlOmronCP1LTest(void)
{
    int i = 0;
    uint16_t read_data_length = 0;
    uint8_t read_data[1024] = {0};
    ControlProtocolType fins_protocol = ControlProtocolFind();
    if (NULL == fins_protocol) {
        printf("%s get fins protocol %p failed\n", __func__, fins_protocol);
        return;
    }

    printf("%s get fins protocol %p successfull\n", __func__, fins_protocol);

    if (CONTROL_REGISTERED == fins_protocol->protocol_status) {
        ControlProtocolOpen(fins_protocol);

        for (;;) {
            read_data_length = ControlProtocolRead(fins_protocol, read_data, sizeof(read_data));
            printf("%s read [%d] fins data %d using receipe file\n", __func__, i, read_data_length);
            i++;
            PrivTaskDelay(100000);
        }

        //ControlProtocolClose(fins_protocol);
    }
}
PRIV_SHELL_CMD_FUNCTION(ControlOmronCP1LTest, Omron Plc Cp1l Demo, PRIV_SHELL_CMD_MAIN_ATTR);