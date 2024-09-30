/**
* iperf-liked network performance tool
*
*/

#include <xizi.h>
#include <shell.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/select.h>
#include <sys_arch.h>
#include "lwip/sockets.h"
#include <netdb.h>
#include <lwiperf.h>

#define IPERF_PORT          5001
#define IPERF_BUFSZ         (4 * 1024)

#define IPERF_MODE_STOP     0
#define IPERF_MODE_SERVER   1
#define IPERF_MODE_CLIENT   2

typedef struct{
    int mode;
    char *host;
    int port;
} IPERF_PARAM;
static IPERF_PARAM param = {IPERF_MODE_STOP, NULL, IPERF_PORT};

char tcp_iperf_ip[] = {192, 168, 130, 77};
char tcp_iperf_mask[] = {255, 255, 254, 0};
char tcp_iperf_gw[] = {192, 168, 130, 1};

static void iperf_udp_client(void *thread_param)
{
    int sock;
    uint32 *buffer;
    struct sockaddr_in server;
    uint32 packet_count = 0;
    uint32 tick;
    int send_size;

    send_size = IPERF_BUFSZ > 1470 ? 1470 : IPERF_BUFSZ;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        KPrintf("[%s:%d] can't create socket! exit!\n", __FILE__, __LINE__);
        return;
    }

    server.sin_family = PF_INET;
    server.sin_port = htons(param.port);
    server.sin_addr.s_addr = inet_addr(param.host);
    memset(&(server.sin_zero), 0, sizeof(server.sin_zero));

    if (connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr))){
        lw_error("Unable to connect\n");
        closesocket(sock);
        return;
    }
    
    buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL){
        printf("[%s:%d] malloc failed\n", __FILE__, __LINE__);
        return;
    }
    memset(buffer, 0x00, IPERF_BUFSZ);

    KPrintf("iperf udp mode run...\n");
    while (param.mode != IPERF_MODE_STOP){
        packet_count++;
        tick = CurrentTicksGain();
        buffer[0] = htonl(packet_count);
        buffer[1] = htonl(tick / TICK_PER_SECOND);
        buffer[2] = htonl((tick % TICK_PER_SECOND) * 1000);
        sendto(sock, buffer, send_size, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
    }
    closesocket(sock);
    free(buffer);
    KPrintf("iperf udp mode exit...\n");
}

static void iperf_udp_server(void *thread_param)
{
    int sock;
    uint32 *buffer;
    struct sockaddr_in server;
    struct sockaddr_in sender;
    int sender_len, r_size;
    uint64 sentlen;
    uint32 pcount = 0, last_pcount = 0;
    uint32 lost, total;
    x_ticks_t tick1, tick2;
    struct timeval timeout;

    buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL){
        return;
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        KPrintf("can't create socket! exit!");
        return;
    }

    server.sin_family = PF_INET;
    server.sin_port = htons(param.port);
    server.sin_addr.s_addr = inet_addr("0.0.0.0");

    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1){
        KPrintf("setsockopt failed!");
        closesocket(sock);
        free(buffer);
        return;
    }

    if (bind(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0){
        KPrintf("iperf server bind failed! exit!");
        closesocket(sock);
        free(buffer);
        return;
    }

    while (param.mode != IPERF_MODE_STOP){
        tick1 = CurrentTicksGain();
        tick2 = tick1;
        lost = 0;
        total = 0;
        sentlen = 0;
        while ((tick2 - tick1) < (TICK_PER_SECOND * 5)){
            r_size = recvfrom(sock, buffer, IPERF_BUFSZ, 0, (struct sockaddr *)&sender, (socklen_t*)&sender_len);
            if (r_size > 12){
                pcount = ntohl(buffer[0]);
                if (last_pcount < pcount){
                    lost += pcount - last_pcount - 1;
                    total += pcount - last_pcount;
                }
                else{
                    last_pcount = pcount;
                }
                last_pcount = pcount;
                sentlen += r_size;
            }
            tick2 = CurrentTicksGain();
        }
        if (sentlen > 0){
            long data;
            int integer, decimal;
            KTaskDescriptorType tid;

            tid = GetKTaskDescriptor();
            data = sentlen * TICK_PER_SECOND / 125 / (tick2 - tick1);
            integer = data/1000;
            decimal = data%1000;
            KPrintf("%s: %d.%03d0 Mbps! lost:%d total:%d\n", tid->task_base_info.name, integer, decimal, lost, total);
        }
    }
    free(buffer);
    closesocket(sock);
}

static void iperf_client(void *thread_param)
{
    int i;
    int sock;
    int ret;
    int tips = 1;
    uint8_t *send_buf;
    uint64 sentlen;
    x_ticks_t tick1, tick2;
    struct sockaddr_in addr;

    send_buf = (uint8_t *) malloc(IPERF_BUFSZ);
    if (!send_buf) return ;

    for (i = 0; i < IPERF_BUFSZ; i ++)
        send_buf[i] = i & 0xff;

    while (param.mode != IPERF_MODE_STOP)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0){
            KPrintf("create socket failed!");
            DelayKTask(TICK_PER_SECOND);
            continue;
        }

        addr.sin_family = PF_INET;
        addr.sin_port = htons(param.port);
        addr.sin_addr.s_addr = inet_addr((char *)param.host);

        ret = connect(sock, (const struct sockaddr *)&addr, sizeof(addr));
        if (ret == -1){
            if (tips){
                KPrintf("Connect to iperf server faile, Waiting for the server to open!");
                tips = 0;
            }
            closesocket(sock);
            DelayKTask(TICK_PER_SECOND);
            continue;
        }
        KPrintf("Connect to iperf server successful!\n");

        {
            int flag = 1;

            setsockopt(sock,
                       IPPROTO_TCP,     /* set option at TCP level */
                       TCP_NODELAY,     /* name of option */
                       (void *) &flag,  /* the cast is historical cruft */
                       sizeof(int));    /* length of option value */
        }

        sentlen = 0;

        tick1 = CurrentTicksGain();
        while (param.mode != IPERF_MODE_STOP){
            tick2 = CurrentTicksGain();
            if (tick2 - tick1 >= TICK_PER_SECOND * 5){
                double speed;
                // int integer, decimal;
                KTaskDescriptorType tid;

                tid = GetKTaskDescriptor();
                speed = (double)(sentlen * TICK_PER_SECOND / 125 / (tick2 - tick1));
                speed = speed / 1000.0f;
                printf("%s: %2.4f Mbps!\n", tid->task_base_info.name, speed);
                tick1 = tick2;
                sentlen = 0;
            }

            ret = send(sock, send_buf, IPERF_BUFSZ, 0);
            if (ret > 0){
                sentlen += ret;
            }

            if (ret < 0) break;
        }

        closesocket(sock);

        DelayKTask(TICK_PER_SECOND * 2);
        KPrintf("Disconnected, iperf server shut down!");
        tips = 1;
    }
    free(send_buf);
}

// iperf tcp server running thread
struct sock_conn_cb {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int connected;
    int parent_id;
};

void iperf_sever_worker(void* arg) {
    struct sock_conn_cb *sccb = (struct sock_conn_cb *)arg;
    x_ticks_t tick1, tick2;

    uint8_t *recv_data = (uint8_t *)malloc(IPERF_BUFSZ);
    if(recv_data == NULL) {
        KPrintf("[%s] No Memory.\n", __func__);
        goto exit__;
    }

    uint64 recvlen = 0;
    int32_t bytes_received = 0;
    
    int flag = 1;
    setsockopt(sccb->connected,
                IPPROTO_TCP,     /* set option at TCP level */
                TCP_NODELAY,     /* name of option */
                (void *) &flag,  /* the cast is historical cruft */
                sizeof(int));    /* length of option value */

    int cur_tid = GetKTaskDescriptor()->id.id;

    tick1 = CurrentTicksGain();
    while (param.mode != IPERF_MODE_STOP){
        bytes_received = recv(sccb->connected, recv_data, IPERF_BUFSZ, 0);
        if (bytes_received == 0) {
            KPrintf("client disconnected (%s, %d)\n",
                inet_ntoa(sccb->client_addr.sin_addr), ntohs(sccb->client_addr.sin_port));
            break;
        } else if (bytes_received < 0) {
            KPrintf("recv error, client: (%s, %d)\n",
                inet_ntoa(sccb->client_addr.sin_addr), ntohs(sccb->client_addr.sin_port));
            break;
        }

        recvlen += bytes_received;

        tick2 = CurrentTicksGain();
        if (tick2 - tick1 >= TICK_PER_SECOND * 5) {
            double speed;
            // int integer, decimal;
            KTaskDescriptorType tid;

            tid = GetKTaskDescriptor();
            speed = (double)(recvlen * TICK_PER_SECOND / (125 * (tick2 - tick1)));
            speed = speed / 1000.0f;
            printf("%s%d: %2.4f Mbps!\n",
                tid->task_base_info.name, cur_tid - sccb->parent_id, speed);
            tick1 = tick2;
            recvlen = 0;
        }
    }
    free(recv_data);

exit__:
    if (sccb->connected >= 0) closesocket(sccb->connected);
    sccb->connected = -1;
    free(sccb);
    KPrintf("iperf server %d quiting.\n", cur_tid - sccb->parent_id);
}

void iperf_server_multithread(void *thread_param)
{
    socklen_t sin_size;
    x_ticks_t tick1, tick2;
    int sock;
    int connected;
    struct sockaddr_in server_addr, client_addr;
    fd_set readset;
    struct timeval timeout;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        KPrintf("[%s:%d] Socket error!\n", __FILE__, __LINE__);
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param.port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
        KPrintf("Unable to bind!\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1){
        KPrintf("Listen error!\n");
        goto __exit;
    }

    int cur_tid = GetKTaskDescriptor()->id.id;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while (param.mode != IPERF_MODE_STOP){
        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        if (select(sock + 1, &readset, NULL, NULL, &timeout) == 0) {
            continue;
        }

        sin_size = sizeof(struct sockaddr_in);

        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        printf("new client connected from (%s, %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        struct sock_conn_cb *sccb = malloc(sizeof(struct sock_conn_cb));
        sccb->connected = connected;
        sccb->client_addr = client_addr;
        sccb->server_addr = server_addr;
        sccb->parent_id = cur_tid;
        int tid = KTaskCreate("iperf server", iperf_sever_worker, sccb, LWIP_TASK_STACK_SIZE, 20);
        // iperf_sever_worker(sccb);
        if (tid) {
            StartupKTask(tid);
        } else {
            KPrintf("[%s] Failed to create server worker.\n", __func__);
            free(sccb);
        }
    }

__exit:
    if (sock >= 0) closesocket(sock);
}

void iperf_server(void *thread_param)
{
    uint8_t *recv_data;
    socklen_t sin_size;
    x_ticks_t tick1, tick2;
    int sock = -1, connected, bytes_received;
    uint64 recvlen;
    struct sockaddr_in server_addr, client_addr;
    fd_set readset;
    struct timeval timeout;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        KPrintf("[%s:%d] Socket error!\n", __FILE__, __LINE__);
        goto __exit;
    }

    recv_data = (uint8_t *)malloc(IPERF_BUFSZ);
    if (recv_data == NULL){
        KPrintf("No memory!\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param.port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
        KPrintf("Unable to bind!\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1){
        KPrintf("Listen error!\n");
        goto __exit;
    }

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (param.mode != IPERF_MODE_STOP){
        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        if (select(sock + 1, &readset, NULL, NULL, &timeout) == 0) {
            continue;
        }

        sin_size = sizeof(struct sockaddr_in);

        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        printf("new client connected from (%s, %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int flag = 1;
        setsockopt(connected,
                    IPPROTO_TCP,     /* set option at TCP level */
                    TCP_NODELAY,     /* name of option */
                    (void *) &flag,  /* the cast is historical cruft */
                    sizeof(int));    /* length of option value */

        recvlen = 0;
        tick1 = CurrentTicksGain();
        while (param.mode != IPERF_MODE_STOP){
            bytes_received = recv(connected, recv_data, IPERF_BUFSZ, 0);
            if (bytes_received == 0) {
                KPrintf("client disconnected (%s, %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            } else if (bytes_received < 0) {
                KPrintf("recv error, client: (%s, %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }

            recvlen += bytes_received;

            tick2 = CurrentTicksGain();
            if (tick2 - tick1 >= TICK_PER_SECOND * 5) {
                double speed;
                // int integer, decimal;
                KTaskDescriptorType tid;

                tid = GetKTaskDescriptor();
                speed = (double)(recvlen * TICK_PER_SECOND / (125 * (tick2 - tick1)));
                speed = speed / 1000.0f;
                printf("%s: %2.4f Mbps!\n", tid->task_base_info.name, speed);
                tick1 = tick2;
                recvlen = 0;
            }
        }
        if (connected >= 0) closesocket(connected);
        connected = -1;
    }

__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) free(recv_data);
}

void iperf_usage(void)
{
    KPrintf("Usage: iperf [-s|-c host] [options] [multi-threaded]\n");
    KPrintf("       iperf [-h|--stop]\n");
    KPrintf("\n");
    KPrintf("Client/Server:\n");
    KPrintf("  -p #         server port to listen on/connect to\n");
    KPrintf("\n");
    KPrintf("Server specific:\n");
    KPrintf("  -s           run in server mode\n");
    KPrintf("\n");
    KPrintf("Client specific:\n");
    KPrintf("  -c <host>    run in client mode, connecting to <host>\n");
    KPrintf("\n");
    KPrintf("Miscellaneous:\n");
    KPrintf("  -h           print this message and quit\n");
    KPrintf("  --stop       stop iperf program\n");
    KPrintf("  -u           testing UDP protocol\n");
    KPrintf("  -m <time>    the number of multi-threaded \ns");
    return;
}

int iperf(int argc, char **argv)
{
    int mode = 0; /* server mode */
    char *host = NULL;
    int port = IPERF_PORT;
    int numtid = 1;
    int use_udp = 0;
    int index = 1;

    if (argc == 1)
    {
        goto __usage;
    }
    if (strcmp(argv[1], "-u") == 0)
    {
        index = 2;
        use_udp = 1;
    }
    if (strcmp(argv[index], "-h") == 0) goto __usage;
    else if (strcmp(argv[index], "--stop") == 0)
    {
        /* stop iperf */
        param.mode = IPERF_MODE_STOP;
        printf("iperf stop.\n");
        return 0;
    }
    else if (strcmp(argv[index], "-s") == 0)
    {
        mode = IPERF_MODE_SERVER; /* server mode */

        /* iperf -s -p 5000 */
        if (argc >= 4)
        {
            if (strcmp(argv[index + 1], "-p") == 0)
            {
                port = atoi(argv[index + 2]);
            }
            else goto __usage;
        }
    }
    else if (strcmp(argv[index], "-c") == 0)
    {
        mode = IPERF_MODE_CLIENT; /* client mode */
        if (argc < 3) goto __usage;

        host = argv[index + 1];
        if (argc >= 5)
        {
            /* iperf -c host -p port */
            if (strcmp(argv[index + 2], "-p") == 0)
            {
                port = atoi(argv[index + 3]);
            }
            else goto __usage;
        }
    }
    else goto __usage;

    if (argc >= 7)
    {
        if(strcmp(argv[argc - 2], "-m") == 0)
        {
            numtid = atoi(argv[argc - 1]);
        }
        else  goto __usage;
    }

    /* start iperf */
    if (param.mode == IPERF_MODE_STOP)
    {
        int i = 0;
        char tid_name[NAME_NUM_MAX + 1] = {0};

        param.mode = mode;
        param.port = port;
        if (param.host)
        {
            free(param.host);
            param.host = NULL;
        }
        if (host) param.host = strdup(host);

        for (i = 0; i < numtid; i++)
        {
            int32 tid = 0;
            void (*function)(void *parameter);

            if (use_udp)
            {
                if (mode == IPERF_MODE_CLIENT)
                {
                    snprintf(tid_name, sizeof(tid_name), "iperfc%02d", i + 1);
                    function = iperf_udp_client;
                }
                else if (mode == IPERF_MODE_SERVER)
                {
                    snprintf(tid_name, sizeof(tid_name), "iperfd%02d", i + 1);
                    function = iperf_udp_server;
                }
            }
            else
            {
                if (mode == IPERF_MODE_CLIENT)
                {
                    snprintf(tid_name, sizeof(tid_name), "iperfc%02d", i + 1);
                    function = iperf_client;
                }
                else if (mode == IPERF_MODE_SERVER)
                {
                    snprintf(tid_name, sizeof(tid_name), "iperfd%02d", i + 1);
                    function = iperf_server_multithread;
                }
            }

            tid = KTaskCreate(tid_name, function, NULL, LWIP_TASK_STACK_SIZE, 20);
            if (tid) StartupKTask(tid);
        }
    }
    else
    {
        KPrintf("Please stop iperf firstly, by:\n");
        KPrintf("iperf --stop\n");
    }

    return 0;

__usage:
    iperf_usage();
    return 0;
}

#if LWIP_TCP
static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(local_addr);
  LWIP_UNUSED_ARG(local_port);
 
  printf("IPERF report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
    (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec);
}
#endif /* LWIP_TCP */
 
 
void
lwiperf_example_init(void)
{
#if LWIP_TCP
  ip4_addr_t ipaddr;
 
 lwiperf_start_tcp_server_default(lwiperf_report, NULL);
 
  // IP4_ADDR(&ipaddr,192,168,0,181);
  // lwiperf_start_tcp_client_default(&ipaddr, lwiperf_report, NULL);
#endif
 
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(8),
    iperf, iperf, netutils iperf);

extern void *lwiperf_start_tcp_server_default(lwiperf_report_fn report_fn, void *report_arg);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(8),
    lwiperf_tcp_server, lwiperf_example_init, netutils lwipperf);
