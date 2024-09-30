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
* @file lwip_udp_socket_demo.c
* @brief UDP demo based on LwIP
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-03-21
*/
#include <transform.h>

#ifdef ADD_XIZI_FEATURES
#include <sys_arch.h>
#include "lwip/sockets.h"
#endif

#ifdef ADD_NUTTX_FEATURES
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define LWIP_DEMO_TIMES 3
#define LWIP_LOCAL_PORT 4840

#define lw_error printf
#define lw_notice printf
#define lw_print printf
#endif

#define UDP_DEMO_BUF_SIZE                    65535
#define UDP_DEMO_SEND_TIMES                  20
#define LWIP_UDP_DEMO_TASK_STACK_SIZE        4096
#define LWIP_UDP_DEMO_TASK_PRIO              20

static pthread_t udp_client_task;
static pthread_t udp_server_task;

static char udp_demo_ipaddr[] = {192, 168, 131, 77};
static char udp_demo_netmask[] = {255, 255, 254, 0};
static char udp_demo_gwaddr[] = {192, 168, 131, 1};

static char udp_ip_str[128] = {0};
static uint16_t udp_socket_port = 8888;

/*****************************************************************************/
void UdpSocketConfigParam(char *ip_str)
{
    int ip1, ip2, ip3, ip4, port = 0;

    if(ip_str == NULL)
        return;

    if(sscanf(ip_str, "%d.%d.%d.%d:%d", &ip1, &ip2, &ip3, &ip4, &port)) {
        printf("config ip %s port %d\n", ip_str, port);
        strcpy(udp_ip_str, ip_str);
        if(port)
            udp_socket_port = port;
        return;
    }

    if(sscanf(ip_str, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4)) {
        printf("config ip %s\n", ip_str);
        strcpy(udp_ip_str, ip_str);
    }
}

static void *UdpSocketRecvTask(void *arg)
{
    int fd = -1;
    char *recv_buf;
    struct sockaddr_in udp_addr, server_addr;
    int recv_len;

    while(1) {
        recv_buf = (char *)PrivMalloc(UDP_DEMO_BUF_SIZE);
        if(recv_buf == NULL) {
            lw_error("No memory\n");
            continue;
        }

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(fd < 0) {
            lw_error("Socket error\n");
            free(recv_buf);
            continue;
        }

        udp_addr.sin_family = AF_INET;
        udp_addr.sin_addr.s_addr = INADDR_ANY;
        udp_addr.sin_port = htons(udp_socket_port);
        memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));

        if(bind(fd, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) == -1) {
            lw_error("Unable to bind\n");
            close(fd);
            free(recv_buf);
            continue;
        }

        lw_notice("UDP bind success, start to receive.\n");
        lw_notice("\n\nLocal Port:%d\n\n", udp_socket_port);

        while(1) {
            memset(recv_buf, 0, UDP_DEMO_BUF_SIZE);
            recv_len = recv(fd, recv_buf, UDP_DEMO_BUF_SIZE, 0);
            if(recv_len > 0) {
                lw_notice("Receive from : %s\n", inet_ntoa(server_addr.sin_addr));
                lw_notice("Receive data : %s\n\n", recv_buf);
            }
            send(fd, recv_buf, recv_len, 0);
        }

        close(fd);
        free(recv_buf);
    }
}

void UdpSocketRecvTest(int argc, char *argv[])
{
    if(argc >= 2) {
        lw_notice("lw: [%s] target ip %s\n", __func__, argv[1]);
        UdpSocketConfigParam(argv[1]);
    }

#ifdef ADD_XIZI_FEATURES
    lwip_config_tcp(0, udp_demo_ipaddr, udp_demo_netmask, udp_demo_gwaddr);

    pthread_attr_t attr;
    attr.schedparam.sched_priority = LWIP_UDP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_UDP_DEMO_TASK_STACK_SIZE;
#endif
#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t attr = PTHREAD_ATTR_INITIALIZER;
    attr.priority = LWIP_UDP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_UDP_DEMO_TASK_STACK_SIZE;
#endif

    PrivTaskCreate(&udp_server_task, &attr, &UdpSocketRecvTask, NULL);
    PrivTaskStartup(&udp_server_task);
}
PRIV_SHELL_CMD_FUNCTION(UdpSocketRecvTest, a udp receive sample, PRIV_SHELL_CMD_MAIN_ATTR);

static void *UdpSocketSendTask(void *arg)
{
    int cnt = UDP_DEMO_SEND_TIMES;
    char send_str[128];
    int fd = -1;

    memset(send_str, 0, sizeof(send_str));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        lw_error("Socket error\n");
        return NULL;
    }

    struct sockaddr_in udp_sock;
    udp_sock.sin_family = AF_INET;
    udp_sock.sin_port = htons(udp_socket_port);
    udp_sock.sin_addr.s_addr = inet_addr(udp_ip_str);
    memset(&(udp_sock.sin_zero), 0, sizeof(udp_sock.sin_zero));

    if(connect(fd, (struct sockaddr *)&udp_sock, sizeof(struct sockaddr)) < 0) {
        lw_error("Unable to connect %s:%d\n", udp_ip_str, udp_socket_port);
        close(fd);
        return NULL;
    }

    lw_print("UDP connect %s:%d success, start to send.\n",
        udp_ip_str,
        udp_socket_port);

    while(cnt --) {
        snprintf(send_str, sizeof(send_str), "UDP test package times %d\r\n", cnt);
        send(fd, send_str, strlen(send_str), 0);
        lw_notice("Send UDP msg: %s ", send_str);
        PrivTaskDelay(1000);
    }

    close(fd);
    return NULL;
}

void UdpSocketSendTest(int argc, char *argv[])
{
    if(argc >= 2) {
        lw_notice("lw: [%s] target ip %s\n", __func__, argv[1]);
        UdpSocketConfigParam(argv[1]);
    }

#ifdef ADD_XIZI_FEATURES
    lwip_config_tcp(0, udp_demo_ipaddr, udp_demo_netmask, udp_demo_gwaddr);

    pthread_attr_t attr;
    attr.schedparam.sched_priority = LWIP_UDP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_UDP_DEMO_TASK_STACK_SIZE;
#endif
#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t attr = PTHREAD_ATTR_INITIALIZER;
    attr.priority = LWIP_UDP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_UDP_DEMO_TASK_STACK_SIZE;
#endif

    PrivTaskCreate(&udp_client_task, &attr, &UdpSocketSendTask, NULL);
    PrivTaskStartup(&udp_client_task);
}
PRIV_SHELL_CMD_FUNCTION(UdpSocketSendTest, a udp send sample, PRIV_SHELL_CMD_MAIN_ATTR);

