/*
* Copyright (c) 2020 AIIT XUOS Lab
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
* @file:    test_can.c
* @brief:   a application of can function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2023/2/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES


void TestCAN(void)
{
    // config CAN pin(SCL:34.SDA:35) in menuconfig
    int can_fd = PrivOpen(CAN_DEV_DRIVER, O_RDWR);
    if (can_fd < 0)
    {
        printf("open can_fd fd error:%d\n", can_fd);
        return;
    }
    printf("CAN open successful!\n");

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = CAN_TYPE;

    struct CanDriverConfigure can_config;
    can_config.brp  = 8U;
    can_config.tbs1 = 1U + 8U;
    can_config.tbs2 = 4U;
    can_config.tsjw = 4U;
    can_config.mode = 0U;

    ioctl_cfg.args = (void *)&can_config;

    if (0 != PrivIoctl(can_fd, OPE_INT, &ioctl_cfg))
    {
        printf("init can fd error %d\n", can_fd);
        PrivClose(can_fd);
        return;
    }
    printf("CAN configure successful!\n");

    uint8_t data_buff[64u] = {1,2,3,4,4,3,2,1};
    struct CanSendConfigure frame_send;
    frame_send.ide=0;
    frame_send.stdid = 0x55;
    frame_send.rtr=0;
    frame_send.data_lenth=8;
    frame_send.data = data_buff;

    struct CanSendConfigure frame_recv;
    uint8_t recv_buff[65U] = {0};
    frame_recv.data = recv_buff;

    // CAN write
    while (1)
    {
        PrivTaskDelay(500);
        PrivWrite(can_fd, &frame_send, NONE);
        PrivTaskDelay(500);
        PrivRead(can_fd, &frame_recv, NONE);
        // if any data has received,Then printf message
        if(frame_recv.data_lenth > 0){
            printf("ID %08x:%s\n",frame_recv.exdid,frame_recv.data);
        }
    }

    PrivClose(can_fd);
    return;
}

PRIV_SHELL_CMD_FUNCTION(TestCAN, a can test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif