/*
* 程序清单：tcp 服务端
 *
 * 这是一个 tcp 服务端的例程
 * 导出 tcp_server 命令到控制终端
 * 命令调用格式：tcp_server
 * 无参数
 * 程序功能：作为一个服务端，接收并显示客户端发来的数据 ，接收到 exit 退出程序
 * Created by Ybeichen on 2022/7/28.
*/

#include "tcp_server.h"
#include <rtthread.h>
#include <wiz_socket.h>  /* 使用BSD socket，需要包含socket.h头文件 */
#include "netdb.h"
#include <rtdef.h>
#include <errno.h>

#define BUFSZ       (2048)
static int port = 5000;
static int is_running = 0; /* 停止标志 */
static const char send_data[] = "This is TCP Server from RT-Thread."; /* 发送用到的数据 */

static void tcp_server(void *argr)
{
    char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求可用内存 */
    socklen_t sin_size;
    int sock, connected, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    int ret;

    recv_data = rt_malloc(BUFSZ + 1); /* 分配接收用的数据缓冲 */
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /*  创建一个socket，类型是SOCKET_STREAM，TCP类型 */
    if ((sock = socket(AF_WIZ, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        /* 创建socket失败 */
        rt_kprintf("Socket error\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 初始化本地服务端地址 */
    server_addr.sin_family = AF_WIZ;
    server_addr.sin_port = htons(port); /* 服务端工作的端口 */
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务端地址 */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 绑定失败 */
        rt_kprintf("Unable to bind\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 在socket上进行监听 */
    if (listen(sock, 5) == -1)
    {
        rt_kprintf("Listen error\n");

        /* release recv buffer */
        rt_free(recv_data);
        return;
    }

    rt_kprintf("\nTCPServer Waiting for client on port %d...\n", port);
    
    is_running=1;
    while (is_running)
    {

        /* 接受一个客户端连接socket的请求，这个函数调用是阻塞式的 */
        connected = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)sizeof(struct sockaddr_in));
        /* 返回的是连接成功的socket */
        if (connected < 0)
        {
            rt_kprintf("accept connection failed! errno = %d\n", errno);
            /* release recv buffer */
            rt_free(recv_data);
            continue;
        }

        /* 接受返回的client_addr指向了客户端的地址信息 */
        rt_kprintf("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        /* 客户端连接的处理 */
        while (is_running)
        {
            /* 发送数据到connected socket */
            ret = send(connected, send_data, strlen(send_data), 0);
            if (ret < 0)
            {
                /* 发送失败，关闭这个连接 */
                rt_kprintf("\nsend error,close the socket.\r\n");
                closesocket(connected);
                /* 释放接收缓冲 */
                rt_free(recv_data);
                break;
            }
            else if (ret == 0)
            {
                /* 打印send函数返回值为0的警告信息 */
                rt_kprintf("\n Send warning,send function return 0.\r\n");
                
            }

            /* 从connected socket中接收数据，接收buffer是1024大小，但并不一定能够收到1024大小的数据 */
            bytes_received = recv(connected, recv_data, BUFSZ, 0);
            if (bytes_received < 0)
            {
                /* 接收失败，关闭这个connected socket */
                rt_kprintf("\nReceived error, close the connect.\r\n");
                closesocket(connected);
                /* 释放接收缓冲 */
                rt_free(recv_data);
                break;
            }
            else if (bytes_received == 0)
            {
                /* 打印recv函数返回值为0的警告信息 */
                rt_kprintf("\nReceived warning,recv function return 0.\r\n");
                closesocket(connected);
                /* 释放接收缓冲 */
                rt_free(recv_data);
                break;
            }

            /* 有接收到数据，把末端清零 */
            recv_data[bytes_received] = '\0';
            if (strncmp(recv_data, "q", 1) == 0 || strncmp(recv_data, "Q", 1) == 0)
            {
                /* 如果是首字母是q或Q，关闭这个连接 */
                rt_kprintf("\nGot a 'q' or 'Q', close the connect.\r\n");
                closesocket(connected);
                /* 释放接收缓冲 */
                rt_free(recv_data);
                break;
            }
            else if (strcmp(recv_data, "exit") == 0)
            {
                /* 如果接收的是exit，则关闭整个服务端 */
                closesocket(connected);
                is_running=0;
                break;
            }
            else
            {
                /* 在控制终端显示收到的数据 */
                rt_kprintf("RECEIVED DATA = %s \n", recv_data);
            }
        }
    }

    /* 退出服务 */
    closesocket(sock);

    /* 释放接收缓冲 */
    rt_free(recv_data);

    return;
}
MSH_CMD_EXPORT(tcp_server, a tcp server sample);