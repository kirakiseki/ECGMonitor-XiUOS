#include "socket.h"
#include "w5500.h"
#include "connect_w5500.h"

#ifdef BSP_WIZ_USE_IPERF

#define IPERF_PORT          5001
#define IPERF_BUFSZ         (4 * 1024)

#define IPERF_MODE_STOP     0
#define IPERF_MODE_SERVER   1
#define IPERF_MODE_CLIENT   2

typedef struct{
    int mode;
    uint8 host[4];
    uint16 port;
} IPERF_PARAM;
static IPERF_PARAM param = {IPERF_MODE_STOP, {0, 0, 0, 0}, IPERF_PORT};

static void iperf_udp_client(void *thread_param)
{
    int sock;
    uint8 *buffer;
    uint16 local_port = 4840;
    uint32 packet_count = 0;
    uint32 tick;
    int send_size;

    send_size = IPERF_BUFSZ > 1470 ? 1470 : IPERF_BUFSZ;

    sock = 0;   // w5500支持8个socket独立工作，todo socket端口管理
    // setSn_TXBUF_SIZE(sock, 16);
    // setSn_RXBUF_SIZE(sock, 16);
    

    buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL){
        printf("[%s:%d] malloc failed\n", __FILE__, __LINE__);
        return;
    }
    for(int i = 0; i < IPERF_BUFSZ; i++)
        buffer[i] = i % 10;

    KPrintf("iperf udp mode run...\n");
    while (param.mode != IPERF_MODE_STOP){
        switch(getSn_SR(sock)){
            case SOCK_CLOSED:
                wiz_socket(sock, Sn_MR_UDP, local_port, 0x00);
                break;
            case SOCK_UDP:
                packet_count++;
                tick = CurrentTicksGain();
                buffer[0] = (uint8)(packet_count >> 24);
                buffer[1] = (uint8)(packet_count >> 16);
                buffer[2] = (uint8)(packet_count >> 8);
                buffer[3] = (uint8)(packet_count);

                buffer[4] = (uint8)((tick / TICK_PER_SECOND) >> 24);
                buffer[5] = (uint8)((tick / TICK_PER_SECOND) >> 16);
                buffer[6] = (uint8)((tick / TICK_PER_SECOND) >> 8);
                buffer[7] = (uint8)((tick / TICK_PER_SECOND));

                buffer[8] = (uint8)(((tick % TICK_PER_SECOND) * 1000) >> 24);
                buffer[9] = (uint8)(((tick % TICK_PER_SECOND) * 1000) >> 16);
                buffer[10] = (uint8)(((tick % TICK_PER_SECOND) * 1000) >> 8);
                buffer[11] = (uint8)(((tick % TICK_PER_SECOND) * 1000));
                wiz_sock_sendto(sock, buffer, send_size, param.host, param.port);
                break;
        }
    }
    if(getSn_SR(sock) != SOCK_CLOSED) wiz_sock_close(sock);
    free(buffer);
    KPrintf("iperf udp mode exit...\n");
}

static void iperf_udp_server(void *thread_param)
{
    int sock, sender_len, r_size;
    uint8 *buffer, client_addr[4];
    uint16 client_port;
    uint32 pcount = 0, last_pcount = 0;
    uint32 lost, total;
    uint64 recvlen;
    x_ticks_t tick1, tick2;
    struct timeval timeout;

    buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL){
        return;
    }

    sock = 0; //todo
    // setSn_RXBUF_SIZE(sock, 16);
    // setSn_TXBUF_SIZE(sock, 16);

    KPrintf("iperf udp server run...\n");
    while (param.mode != IPERF_MODE_STOP){
        tick1 = CurrentTicksGain();
        tick2 = tick1;
        lost = 0;
        total = 0;
        recvlen = 0;
        while ((tick2 - tick1) < (TICK_PER_SECOND * 5)){
            switch(getSn_SR(sock)){
                case SOCK_UDP:
                    if ((r_size = getSn_RX_RSR(sock)) > 0){
                        if (r_size > IPERF_BUFSZ) r_size = IPERF_BUFSZ;
                        memset(buffer, 0, IPERF_BUFSZ);
                        wiz_sock_recvfrom(sock, buffer, r_size, client_addr, &client_port);
                        recvlen += r_size;
                        last_pcount = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
                        if(last_pcount > pcount){
                            total += last_pcount - pcount;
                            lost += last_pcount - pcount - 1;
                            pcount = last_pcount;
                        }
                        else if(last_pcount < 10){
                            pcount = last_pcount;
                            total = 1;
                            lost = 0;
                        }
                    }
                    tick2 = CurrentTicksGain();
                    break;
                case SOCK_CLOSED:
                    wiz_socket(sock, Sn_MR_UDP, param.port, 0x00);
                    break;
            }
        }
        if (recvlen > 0){
            long data;
            int integer, decimal;
            KTaskDescriptorType tid;

            tid = GetKTaskDescriptor();
            data = recvlen * TICK_PER_SECOND / 125 / (tick2 - tick1);
            integer = data/1000;
            decimal = data%1000;
            KPrintf("%s: %d.%03d0 Mbps! recv:%d lost:%d total:%d\n", tid->task_base_info.name, integer, decimal, total - lost, lost, total);
        }
    }
    free(buffer);
    if(getSn_SR(sock) != SOCK_CLOSED) wiz_sock_close(sock);
}

static void iperf_client(void *thread_param)
{
    int i;
    int sock = 0;
    int ret;
    uint8_t *send_buf, connected = 0;
    uint64 sentlen;
    x_ticks_t tick1, tick2;

    // setSn_RXBUF_SIZE(sock, 16);
    // setSn_TXBUF_SIZE(sock, 16);

    send_buf = (uint8_t *) malloc(IPERF_BUFSZ);
    if (!send_buf) return ;

    for (i = 0; i < IPERF_BUFSZ; i ++)
        send_buf[i] = i & 0xff;

    while (param.mode != IPERF_MODE_STOP)
    {
        while((getSn_SR(sock) != SOCK_ESTABLISHED || !connected) && param.mode != IPERF_MODE_STOP){
            switch (getSn_SR(sock)) {
                case SOCK_ESTABLISHED:
                    if (getSn_IR(sock) & Sn_IR_CON) {
                        KPrintf("Connected\n", sock);
                        setSn_IR(sock, Sn_IR_CON);
                    }
                    connected = 1;
                    break;
                case SOCK_CLOSE_WAIT:
                    wiz_sock_disconnect(sock);
                    break;
                case SOCK_INIT:
                    KPrintf("Socket %d:try to connect to [%d.%d.%d.%d:%d]...", 
                        sock, param.host[0], param.host[1], param.host[2], param.host[3], param.port);
                    ret = wiz_sock_connect(sock, param.host, param.port);
                    if (ret != SOCK_OK){
                        printf("failed, wait 1s to try again\n");
                        MdelayKTask(1000);
                    }
                    break;
                case SOCK_CLOSED:
                    if(connected) KPrintf("Socket %d:closed\n", sock);
                    wiz_socket(sock, Sn_MR_TCP, param.port, 0x00);
                    connected = 0;
                    break;
                default:
                    break;
            }
        }

        sentlen = 0;

        tick1 = CurrentTicksGain();
        while (param.mode != IPERF_MODE_STOP){
            tick2 = CurrentTicksGain();
            if (tick2 - tick1 >= TICK_PER_SECOND * 5){
                long data;
                int integer, decimal;
                KTaskDescriptorType tid;

                tid = GetKTaskDescriptor();
                data = sentlen * TICK_PER_SECOND / 125 / (tick2 - tick1);
                integer = data/1000;
                decimal = data%1000;
                KPrintf("%s: %d.%03d0 Mbps!\n", tid->task_base_info.name, integer, decimal);
                tick1 = tick2;
                sentlen = 0;
            }

            ret = wiz_sock_send(sock, send_buf, IPERF_BUFSZ);
            if (ret > 0){
                sentlen += ret;
            }

            if (ret < 0) break;
        }

        if(getSn_SR(sock) != SOCK_CLOSED)wiz_sock_close(sock);

        KPrintf("Disconnected, iperf client exit!");
    }
    free(send_buf);
}

static void iperf_server(void *thread_param)
{
    uint8_t *recv_data;
    x_ticks_t tick1, tick2;
    int sock = -1, connected = 0, bytes_received;
    uint64 recvlen;

    sock = 0; //todo
    // setSn_RXBUF_SIZE(sock, 16);
    // setSn_TXBUF_SIZE(sock, 16);

    recv_data = (uint8_t *)malloc(IPERF_BUFSZ);
    if (recv_data == NULL){
        KPrintf("No memory!\n");
        goto __exit;
    }

    while (param.mode != IPERF_MODE_STOP){
        while((getSn_SR(sock) != SOCK_ESTABLISHED || !connected)){
            switch (getSn_SR(sock)) {
                case SOCK_ESTABLISHED:
                    if (getSn_IR(sock) & Sn_IR_CON) {
                        KPrintf("Socket %d:Connected\n", sock);
                        setSn_IR(sock, Sn_IR_CON);
                    }
                    recvlen = 0;
                    tick1 = CurrentTicksGain();
                    connected = 1;
                    break;
                case SOCK_CLOSE_WAIT:
                    wiz_sock_disconnect(sock);
                    break;
                case SOCK_INIT:
                    KPrintf("Socket %d:Listen, port [%d]\n", sock, param.port);
                    wiz_sock_listen(sock);
                    break;
                case SOCK_CLOSED:
                    if(connected) KPrintf("Socket %d:closed\n", sock);
                    wiz_socket(sock, Sn_MR_TCP, param.port, 0x00);
                    connected = 0;
                    break;
                default:
                    break;
            }
            if(param.mode == IPERF_MODE_STOP) goto __exit;
        }
        
        if ((bytes_received = getSn_RX_RSR(sock)) > 0) {
            if (bytes_received > IPERF_BUFSZ) bytes_received = IPERF_BUFSZ;
            memset(recv_data, 0, IPERF_BUFSZ);
            wiz_sock_recv(sock, recv_data, bytes_received);
            recvlen += bytes_received;
        }            

        tick2 = CurrentTicksGain();
        if (tick2 - tick1 >= TICK_PER_SECOND * 5){
            long data;
            int integer, decimal;
            KTaskDescriptorType tid;

            tid = GetKTaskDescriptor();
            data = recvlen * TICK_PER_SECOND / 125 / (tick2 - tick1);
            integer = data/1000;
            decimal = data%1000;
            KPrintf("%s: %d.%03d0 Mbps!\n", tid->task_base_info.name, integer, decimal);
            tick1 = tick2;
            recvlen = 0;
        }
    }

__exit:
    if(getSn_SR(sock) != SOCK_CLOSED)wiz_sock_close(sock);
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
        MdelayKTask(10);
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
        if (host) {
            uint32 t1,t2,t3,t4;
            sscanf(host, "%d.%d.%d.%d", &t1, &t2, &t3, &t4);
            param.host[0] = (uint8)t1;
            param.host[1] = (uint8)t2;
            param.host[2] = (uint8)t3;
            param.host[3] = (uint8)t4;
        }

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
                    function = iperf_server;
                }
            }

            tid = KTaskCreate(tid_name, function, NULL, 4096, 25);
            if (tid) StartupKTask(tid);
            MdelayKTask(10);
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
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 iperf, iperf,
                 iperf throughput test);
#endif
