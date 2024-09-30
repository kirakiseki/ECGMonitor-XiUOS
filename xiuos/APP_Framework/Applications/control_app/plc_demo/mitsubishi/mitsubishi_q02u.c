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
 * @file mitsubishi_q02u.c
 * @brief PLC MITSUBISHI Q02U app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.9.27
 */

#include <control.h>

extern int Adapter4GActive(void);

void ControlQ02uTest(void)
{
    int i, j = 0;
    int read_data_length = 0;
    uint8_t read_data[128] = {0};

#ifdef CONNECTION_ADAPTER_4G
    Adapter4GActive();
#endif

    ControlProtocolType melsec_3e_protocol = ControlProtocolFind();
    if (NULL == melsec_3e_protocol) {
        printf("%s get melsec 3e protocol %p failed\n", __func__, melsec_3e_protocol);
        return;
    }

    printf("%s get melsec 3e protocol %p successfull\n", __func__, melsec_3e_protocol);

    if (CONTROL_REGISTERED == melsec_3e_protocol->protocol_status) {
        ControlProtocolOpen(melsec_3e_protocol);

        for (;;) {
            read_data_length = ControlProtocolRead(melsec_3e_protocol, read_data, sizeof(read_data));
            printf("%s read [%d] melsec 3c data %d using receipe file\n", __func__, i, read_data_length);
            if (read_data_length) {
                for (j = 0; j < read_data_length; j ++) {
                    printf("j %d data 0x%x\n", j, read_data[j]);
                }
            }
            i++;
            memset(read_data, 0, sizeof(read_data));
            PrivTaskDelay(10000);
        }

        //ControlProtocolClose(melsec_3c_protocol);
    }
}
PRIV_SHELL_CMD_FUNCTION(ControlQ02uTest, Mitsubishi Q02U Demo, PRIV_SHELL_CMD_MAIN_ATTR);


