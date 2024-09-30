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
* @file lwip_tcp_socket_demo.c
* @brief TCP socket demo based on LwIP
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-03-21
*/

#include <transform.h>

#ifdef ADD_XIZI_FEATURES
#include <sys_arch.h>
#include <lwip/sockets.h>
#include "lwip/sys.h"
#endif

#ifdef ADD_NUTTX_FEATURES
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "stdio.h"
#endif

#define TCP_DEMO_BUF_SIZE                    65535
#define TCP_DEMO_SEND_TIMES                  20
#define LWIP_TCP_DEMO_TASK_STACK_SIZE        4096
#define LWIP_TCP_DEMO_TASK_PRIO              20

static pthread_t tcp_client_task;
static pthread_t tcp_server_task;

static char tcp_demo_ipaddr[] = {192, 168, 131, 77};
static char tcp_demo_netmask[] = {255, 255, 254, 0};
static char tcp_demo_gwaddr[] = {192, 168, 131, 1};

#ifdef ADD_NUTTX_FEATURES
#define lw_print printf
#define lw_notice printf
#define lw_error printf

#define LWIP_DEMO_TIMES 3
#define LWIP_TARGET_PORT 4840
#endif

static uint16_t tcp_socket_port = 8888;
static char tcp_ip_str[128] = {0};

/******************************************************************************/
void TcpSocketConfigParam(char *ip_str)
{
    int ip1, ip2, ip3, ip4, port = 0;

    if(ip_str == NULL)
        return;

    if(sscanf(ip_str, "%d.%d.%d.%d:%d", &ip1, &ip2, &ip3, &ip4, &port)) {
        printf("config ip %s port %d\n", ip_str, port);
        strcpy(tcp_ip_str, ip_str);
        if(port)
            tcp_socket_port = port;
        return;
    }

    if(sscanf(ip_str, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4)) {
        printf("config ip %s\n", ip_str);
        strcpy(tcp_ip_str, ip_str);
    }
}

static void *TcpSocketRecvTask(void *arg)
{
    int fd = -1, clientfd;
    int recv_len;
    char *recv_buf;
    struct sockaddr_in tcp_addr;
    socklen_t addr_len;

    while(1) {
        recv_buf = (char *)malloc(TCP_DEMO_BUF_SIZE);
        if (recv_buf == NULL) {
            lw_error("No memory\n");
            continue;
        }

        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            lw_error("Socket error\n");
            free(recv_buf);
            continue;
        }

        tcp_addr.sin_family = AF_INET;
        tcp_addr.sin_addr.s_addr = INADDR_ANY;
        tcp_addr.sin_port = htons(tcp_socket_port);
        memset(&(tcp_addr.sin_zero), 0, sizeof(tcp_addr.sin_zero));

        if (bind(fd, (struct sockaddr *)&tcp_addr, sizeof(struct sockaddr)) == -1) {
            lw_error("Unable to bind\n");
            close(fd);
            free(recv_buf);
            continue;
        }

        lw_print("tcp bind success, start to receive.\n");
        lw_notice("\nLocal Port:%d\n", tcp_socket_port);

        // setup socket fd as listening mode
        if (listen(fd, 5) != 0 ) {
            lw_error("Unable to listen\n");
            close(fd);
            free(recv_buf);
            continue;
        }

        // accept client connection
        clientfd = accept(fd, (struct sockaddr *)&tcp_addr, (socklen_t*)&addr_len);
        lw_notice("client %s connected\n", inet_ntoa(tcp_addr.sin_addr));

        while(1) {
            memset(recv_buf, 0, TCP_DEMO_BUF_SIZE);
            recv_len = recvfrom(clientfd, recv_buf, TCP_DEMO_BUF_SIZE, 0,
                (struct sockaddr *)&tcp_addr, &addr_len);
            if(recv_len > 0) {
                lw_notice("Receive from : %s\n", inet_ntoa(tcp_addr.sin_addr));
                lw_notice("Receive data : %d - %s\n\n", recv_len, recv_buf);
            }
            sendto(clientfd, recv_buf, recv_len, 0, (struct sockaddr*)&tcp_addr, addr_len);
        }
    }

    close(fd);
    free(recv_buf);
}

void TcpSocketRecvTest(int argc, char *argv[])
{
    if(argc >= 2) {
        lw_print("lw: [%s] target ip %s\n", __func__, argv[1]);
        TcpSocketConfigParam(argv[1]);
    }

#ifdef ADD_XIZI_FEATURES
    lwip_config_tcp(0, tcp_demo_ipaddr, tcp_demo_netmask, tcp_demo_gwaddr);

    pthread_attr_t attr;
    attr.schedparam.sched_priority = LWIP_TCP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_TCP_DEMO_TASK_STACK_SIZE;
#endif

#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t attr = PTHREAD_ATTR_INITIALIZER;
    attr.priority = LWIP_TCP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_TCP_DEMO_TASK_STACK_SIZE;
#endif

    PrivTaskCreate(&tcp_server_task, &attr, &TcpSocketRecvTask, NULL);
    PrivTaskStartup(&tcp_server_task);
}
PRIV_SHELL_CMD_FUNCTION(TcpSocketRecvTest, a tcp receive sample, PRIV_SHELL_CMD_MAIN_ATTR);

static void *TcpSocketSendTask(void *arg)
{
    int cnt = TCP_DEMO_SEND_TIMES;
    int fd = -1;
    int ret;
    char send_msg[128];

	lw_print("2022-10-14 Mr. Wang commit TCP\n");
    lw_print("%s start\n", __func__);

    memset(send_msg, 0, sizeof(send_msg));
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        lw_print("Socket error\n");
        return NULL;
    }

    struct sockaddr_in tcp_sock;
    tcp_sock.sin_family = AF_INET;
    tcp_sock.sin_port = htons(tcp_socket_port);
    tcp_sock.sin_addr.s_addr = inet_addr(tcp_ip_str);

    memset(&(tcp_sock.sin_zero), 0, sizeof(tcp_sock.sin_zero));

    ret = connect(fd, (struct sockaddr *)&tcp_sock, sizeof(struct sockaddr));
    if (ret < 0) {
        lw_print("Unable to connect %s:%d = %d\n", tcp_ip_str, tcp_socket_port, ret);
        close(fd);
        return NULL;
    }

    lw_print("TCP connect %s:%d success, start to send.\n", tcp_ip_str, tcp_socket_port);

    while (cnt --) {
        lw_print("Lwip client is running.\n");
        snprintf(send_msg, sizeof(send_msg), "TCP test package times %d\r\n", cnt);
        send(fd, send_msg, strlen(send_msg), 0);
        lw_notice("Send tcp msg: %s ", send_msg);
        PrivTaskDelay(1000);
    }

    close(fd);
    return NULL;
}

void TcpSocketSendTest(int argc, char *argv[])
{
    if(argc >= 2) {
        lw_print("lw: [%s] target ip %s\n", __func__, argv[1]);
        TcpSocketConfigParam(argv[1]);
    }

#ifdef ADD_XIZI_FEATURES
    lwip_config_tcp(0, tcp_demo_ipaddr, tcp_demo_netmask, tcp_demo_gwaddr);

    pthread_attr_t attr;
    attr.schedparam.sched_priority = LWIP_TCP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_TCP_DEMO_TASK_STACK_SIZE;
#endif
#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t attr = PTHREAD_ATTR_INITIALIZER;
    attr.priority = LWIP_TCP_DEMO_TASK_PRIO;
    attr.stacksize = LWIP_TCP_DEMO_TASK_STACK_SIZE;
#endif

    PrivTaskCreate(&tcp_client_task, &attr, &TcpSocketSendTask, NULL);
    PrivTaskStartup(&tcp_client_task);
}
PRIV_SHELL_CMD_FUNCTION(TcpSocketSendTest, a tcp send sample, PRIV_SHELL_CMD_MAIN_ATTR);

