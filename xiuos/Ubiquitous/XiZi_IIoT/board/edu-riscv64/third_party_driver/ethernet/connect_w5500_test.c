#include <dev_pin.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"

extern uint32_t get_gbuf_size();
extern wiz_NetInfo *get_gnetinfo();

enum TCP_OPTION {
  SEND_DATA = 0,
  RECV_DATA,
};

uint32_t wiz_client_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
                       uint8_t dst_ip[4], uint16_t dst_port,
                       enum TCP_OPTION opt) {
  // assert(buf_size <= get_gbuf_size());
  int32_t ret;
  switch (getSn_SR(sn)) {
    case SOCK_CLOSE_WAIT:
      wiz_sock_disconnect(sn);
      break;
    case SOCK_CLOSED:
      wiz_socket(sn, Sn_MR_TCP, 5000, 0x00);
      break;
    case SOCK_INIT:
      KPrintf("[SOCKET CLIENT] sock init.\n");
      wiz_sock_connect(sn, dst_ip, dst_port);
      break;
    case SOCK_ESTABLISHED:
      if (getSn_IR(sn) & Sn_IR_CON) {
        printf("[SOCKET CLIENT] %d:Connected\r\n", sn);
        setSn_IR(sn, Sn_IR_CON);
      }
      if (opt == SEND_DATA) {
        uint32_t sent_size = 0;
        ret = wiz_sock_send(sn, buf, buf_size);
        if (ret < 0) {
          wiz_sock_close(sn);
          return ret;
        }
      } else if (opt == RECV_DATA) {
        uint32_t size = 0;
        if ((size = getSn_RX_RSR(sn)) > 0) {
          if (size > buf_size) size = buf_size;
          ret = wiz_sock_recv(sn, buf, size);
          if (ret <= 0) return ret;
        }
      }
      break;
    default:
      break;
  }
}

void wiz_client_op_test(int argc, char *argv[]) {
  /* argv[1]: ip      ip addr
   * argv[2]: port    port number
   * argv[3]: msg     send msg
   * argv[4]: count   test times,if no this parameter,default 10 times
   */
  if (argc < 4)
  {
    KPrintf("wiz_client_op_test error\n");
    return;
  }
  uint8_t client_sock = 2;
  uint32_t tmp_ip[4];
  uint8_t ip[4];
  uint64_t pCount = 10;
  uint8_t buf[get_gbuf_size()];
  uint16_t port;
 
  sscanf(argv[1], "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
  ip[0] = (uint8_t)tmp_ip[0];
  ip[1] = (uint8_t)tmp_ip[1];
  ip[2] = (uint8_t)tmp_ip[2];
  ip[3] = (uint8_t)tmp_ip[3];
  
  port = atoi(argv[2]);
  KPrintf("wiz client to wiz_server, send to %d.%d.%d.%d %d\n",  // tip info
          ip[0], ip[1], ip[2], ip[3], port);

  if (argc >= 5){
    pCount = atoi(argv[4]);
  }
  for(uint64_t i = 0; i < pCount; i++)
  {
    wiz_client_op(client_sock, argv[3], strlen(argv[3]), ip, port, SEND_DATA);
    MdelayKTask(10);
    // waiting for a responding.
    wiz_client_op(client_sock, buf, get_gbuf_size(), ip, port, RECV_DATA);
    KPrintf("received msg: %s\n", buf);
    memset(buf, 0, get_gbuf_size());
  }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 wiz_client_op, wiz_client_op_test, 
                 wiz_sock_recv or wiz_sock_send data as tcp client);

int32_t wiz_server_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
                      uint16_t port, enum TCP_OPTION opt) {
  int32_t ret = 0;
  uint16_t size = 0, sentsize = 0;
  switch (getSn_SR(sn)) {
    case SOCK_ESTABLISHED:
      if (getSn_IR(sn) & Sn_IR_CON) {
        printf("%d:Connected\r\n", sn);
        setSn_IR(sn, Sn_IR_CON);
      }
      if (opt == SEND_DATA) {
        uint32_t sent_size = 0;
        ret = wiz_sock_send(sn, buf, buf_size);
        if (ret < 0) {
          wiz_sock_close(sn);
          return ret;
        }
      } else if (opt == RECV_DATA) {
        uint32_t size = 0;
        if ((size = getSn_RX_RSR(sn)) > 0) {
          if (size > buf_size) size = buf_size;
          ret = wiz_sock_recv(sn, buf, size);
          return ret;
        }
      }
      break;
    case SOCK_CLOSE_WAIT:
      printf("%d:CloseWait\r\n", sn);
      if ((ret = wiz_sock_disconnect(sn)) != SOCK_OK) return ret;
      printf("%d:Closed\r\n", sn);
      break;
    case SOCK_INIT:
      printf("%d:Listen, port [%d]\r\n", sn, port);
      if ((ret = wiz_sock_listen(sn)) != SOCK_OK) return ret;
      break;
    case SOCK_CLOSED:
      printf("%d:LBTStart\r\n", sn);
      if ((ret = wiz_socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
      printf("%d:Opened\r\n", sn);
      break;
    default:
      break;
  }
  return 0;
}

void wiz_server(void *param) {
  uint16_t port = *(uint16_t *)param;
  KPrintf("wiz server, listen port: %d\n", port);
  uint8_t buf[get_gbuf_size()];
  memset(buf, 0, get_gbuf_size());
  int ret = 0;
  uint32_t size = 0;

  while (1) {
    ret = wiz_server_op(0, buf, get_gbuf_size(), port, RECV_DATA);
    while(buf[size] != 0){
      size ++;
    }
    if (ret > 0) {
      KPrintf("received %d bytes: %s\n", size, buf);

      wiz_server_op(0, buf, get_gbuf_size(), port, SEND_DATA);
      memset(buf, 0, get_gbuf_size());
    }
    size = 0;
  }
}

void wiz_server_test(uint16_t port) {
  /* argv[1]: port
   */
  int32 wiz_server_id =
      KTaskCreate("wiz_server", wiz_server, (void *)&port, 4096, 25);
  x_err_t flag = StartupKTask(wiz_server_id);
  if (flag != EOK) {
    KPrintf("StartupKTask wiz_server_id failed .\n");
  }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) |
                     SHELL_CMD_PARAM_NUM(1),
                 wiz_server_op, wiz_server_test,
                 wiz_sock_recv or wiz_sock_send data as tcp server);

void loopback_udps(int argc, char *argv[]) 
{
  /* argv[1]: remote_ip      ip address of remote udp
   * argv[2]: remote_port    the port number of the remote udp
   * argv[2]: local_port     the port number of the local udp
   */
  uint32_t tmp_ip[4];
  uint8_t remote_ip[4];
  uint16_t remote_port, local_port;
  uint8_t buffer[get_gbuf_size()];
  uint16_t len = 0;

  if (argc < 4)
  {
    KPrintf("loopback_udps test error\n");
    return;
  }

  sscanf(argv[1], "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
  for(int i = 0;i < 4; i++)
  {
    remote_ip[i] = (uint8_t)tmp_ip[i];
  }

  remote_port = atoi(argv[2]);
  local_port = atoi(argv[3]);
  while(1)
  {
    switch (getSn_SR(0)) 
    {
      case SOCK_UDP:
        if(getSn_IR(0) & Sn_IR_RECV)
        {
          setSn_IR(0, Sn_IR_RECV);
        }
        if((len = getSn_RX_RSR(0))>0)
        {
          memset(buffer, 0, len + 1);
          wiz_sock_recvfrom(0, buffer, len, remote_ip, (uint16_t *)&remote_port);
          printf("received msg: %s\n", buffer);
          wiz_sock_sendto(0, buffer, len, remote_ip, remote_port);
        }
        break;

      case SOCK_CLOSED:
        printf("LBUStart\r\n");
        wiz_socket(0, Sn_MR_UDP, local_port, 0);
        break;
    }
  }
}

/* wiz_udp remote_ip remote_port local_port */
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 wiz_udp, loopback_udps, w5500 upd test);

void ifconfig() {
  wiz_NetInfo wiz_netinfo;
  ctlnetwork(CN_GET_NETINFO, (void *)&wiz_netinfo);
  uint8_t tmpstr[6];
  ctlwizchip(CW_GET_ID, (void *)tmpstr);
  KPrintf("=== %s NET CONF ===\r\n", (char *)tmpstr);
  KPrintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", wiz_netinfo.mac[0],
          wiz_netinfo.mac[1], wiz_netinfo.mac[2], wiz_netinfo.mac[3],
          wiz_netinfo.mac[4], wiz_netinfo.mac[5]);
  KPrintf("SIP: %d.%d.%d.%d\r\n", wiz_netinfo.ip[0], wiz_netinfo.ip[1],
          wiz_netinfo.ip[2], wiz_netinfo.ip[3]);
  KPrintf("GAR: %d.%d.%d.%d\r\n", wiz_netinfo.gw[0], wiz_netinfo.gw[1],
          wiz_netinfo.gw[2], wiz_netinfo.gw[3]);
  KPrintf("SUB: %d.%d.%d.%d\r\n", wiz_netinfo.sn[0], wiz_netinfo.sn[1],
          wiz_netinfo.sn[2], wiz_netinfo.sn[3]);
  KPrintf("DNS: %d.%d.%d.%d\r\n", wiz_netinfo.dns[0], wiz_netinfo.dns[1],
          wiz_netinfo.dns[2], wiz_netinfo.dns[3]);
  KPrintf("======================\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 ifconfig, ifconfig, printf w5500 configurations);

void char_arr_assign(uint8_t *dst, uint32_t *src, int len) {
  for (int i = 0; i < len; ++i) {
    dst[i] = (uint8_t)(src[i]);
  }
}

char *network_param_name[] = {"ip", "sn", "gw"};

void config_w5500_network(int argc, char *argv[]) {
  if (argc < 2) {
    KPrintf("[W5500] Network config require params.\n");
    return;
  }

  wiz_NetInfo wiz_netinfo;
  memcpy(&wiz_netinfo, get_gnetinfo(), sizeof(wiz_NetInfo));

  int cur_arg_idx = 1;

  while (argv[cur_arg_idx] != NULL) {
    if (argv[cur_arg_idx + 1] == NULL) {
      KPrintf("[W5500] Network config %s requires value.\n", argv[cur_arg_idx]);
      return;
    }
    uint32_t tmp_arr[4];
    sscanf(argv[cur_arg_idx + 1], "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1],
           &tmp_arr[2], &tmp_arr[3]);
    if (memcmp(argv[cur_arg_idx], network_param_name[0], 2 * sizeof(char)) ==
        0) {
      char_arr_assign(wiz_netinfo.ip, tmp_arr, 4);
    } else if (memcmp(argv[cur_arg_idx], network_param_name[1],
                      2 * sizeof(char)) == 0) {
      char_arr_assign(wiz_netinfo.sn, tmp_arr, 4);
    } else if (memcmp(argv[cur_arg_idx], network_param_name[2],
                      2 * sizeof(char)) == 0) {
      char_arr_assign(wiz_netinfo.gw, tmp_arr, 4);
    } else {
      KPrintf("[W5500] Invalid network param.\n");
    }
    cur_arg_idx += 2;
  }

  ctlnetwork(CN_SET_NETINFO, (void *)&wiz_netinfo);
  KPrintf("[W5500] Network config success.\n", argv[cur_arg_idx]);
  ifconfig();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 config_w5500_network, config_w5500_network,
                 set w5500 configurations);