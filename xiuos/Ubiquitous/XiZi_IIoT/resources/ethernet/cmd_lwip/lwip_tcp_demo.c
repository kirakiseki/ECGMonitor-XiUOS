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
* @file lwip_tcp_demo.c
* @brief TCP demo based on LwIP
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-03-21
*/

#include "board.h"
#include "sys_arch.h"
#include <shell.h>
#include <sys.h>
#include <xizi.h>
#include "lwip_demo.h"
#include "lwip/sockets.h"
#include "tcpecho_raw.h"
char tcp_demo_msg[LWIP_TEST_MSG_SIZE] = { 0 };
char tcp_demo_ip[] = {192, 168, 250, 252};
u16_t tcp_demo_port = 80;
int tcp_send_num = 0;
int tcp_send_task_on = 0;
uint32 tcp_interval = 50;

/******************************************************************************/

static void LwipTcpSendTask(void *arg)
{
    int fd = -1;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        lw_error("Socket error\n");
        return;
    }

    LwipTcpSocketParamType *param = (LwipTcpSocketParamType *)arg;

    struct sockaddr_in tcp_sock;
    tcp_sock.sin_family = AF_INET;
    tcp_sock.sin_port = htons(param->port);
    tcp_sock.sin_addr.s_addr = PP_HTONL(LWIP_MAKEU32(param->ip[0], param->ip[1], param->ip[2], param->ip[3]));
    memset(&(tcp_sock.sin_zero), 0, sizeof(tcp_sock.sin_zero));

    if (connect(fd, (struct sockaddr *)&tcp_sock, sizeof(struct sockaddr)))
    {
        lw_error("Unable to connect\n");
        closesocket(fd);
        return;
    }

    lw_notice("tcp connect success, start to send.\n");
    lw_notice("\n\nTarget Port:%d\n\n", tcp_sock.sin_port);
    tcp_send_task_on = 1;

    while(tcp_send_num > 0 || tcp_send_num == -1)
    {
        sendto(fd, tcp_demo_msg, strlen(tcp_demo_msg), 0, (struct sockaddr*)&tcp_sock, sizeof(struct sockaddr));
        lw_notice("Send tcp msg: %s \n", tcp_demo_msg);
        if(tcp_send_num > 0)
        {
            tcp_send_num--;
        }
        DelayKTask(tcp_interval);
    }

    closesocket(fd);
    tcp_send_task_on = 0;
    return;
}

void LwipTcpSendTest(int argc, char *argv[])
{
    if(tcp_send_task_on){
        tcp_send_num = 0;
        printf("waitting send task exit...\n");
        while(tcp_send_task_on){
            DelayKTask(1000);
        }
        tcp_send_num = 1;
    }
    LwipTcpSocketParamType param;
    uint8_t enet_port = 0;
    memset(tcp_demo_msg, 0, LWIP_TEST_MSG_SIZE);
    if(argc >= 2)
    {
        strncpy(tcp_demo_msg, argv[1], strlen(argv[1]));
    }
    else
    {
        strncpy(tcp_demo_msg, "hello world", strlen("hello world"));
    }
    strcat(tcp_demo_msg, "\r\n");

    if(argc >= 3)
    {
        if(sscanf(argv[2], "%d.%d.%d.%d:%d", &tcp_demo_ip[0], &tcp_demo_ip[1], &tcp_demo_ip[2], &tcp_demo_ip[3], &tcp_demo_port) == EOK)
        {
            sscanf(argv[2], "%d.%d.%d.%d", &tcp_demo_ip[0], &tcp_demo_ip[1], &tcp_demo_ip[2], &tcp_demo_ip[3]);
        }
        sscanf(argv[3], "%d", &tcp_send_num);
        sscanf(argv[4], "%d", &tcp_interval);
    }
    lw_notice("get ipaddr %d.%d.%d.%d:%d send msg %d times\n", tcp_demo_ip[0], tcp_demo_ip[1], tcp_demo_ip[2], tcp_demo_ip[3], tcp_demo_port, tcp_send_num);
    lwip_config_tcp(enet_port, lwip_ipaddr, lwip_netmask, tcp_demo_ip);

    memcpy(param.ip, tcp_demo_ip, 4);
    param.port = tcp_demo_port;
    param.buf = malloc(LWIP_TEST_MSG_SIZE);
    memcpy(param.buf, tcp_demo_msg, LWIP_TEST_MSG_SIZE);

    sys_thread_new("tcp send", LwipTcpSendTask, &param, LWIP_TASK_STACK_SIZE, LWIP_DEMO_TASK_PRIO);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(3),
     TCPSend, LwipTcpSendTest, TCPSend msg [ip:port [num [interval]]]);

void LwipTcpRecvTest(void)
{
    uint8_t enet_port = 0; ///< test enet port 0

    lwip_config_net(enet_port, lwip_ipaddr, lwip_netmask, lwip_gwaddr);
    tcpecho_raw_init();
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(0),
    TCPRecv, LwipTcpRecvTest, TCP Recv message);

