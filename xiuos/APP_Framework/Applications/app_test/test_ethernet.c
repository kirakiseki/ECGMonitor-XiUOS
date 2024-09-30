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
* @file:    test_ethernet.c
* @brief:   a application of ethernet function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#include <socket.h>

#define BUFF_SIZE 128
#define RECV_SIZE 16
#define TCP_PORT  12345

const static uint32_t sn = 0;

#ifdef ETHERNET_AS_SERVER
static int32_t wiz_server_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
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
        memset(buf,0,buf_size);
        strcpy(buf,"The message has been recved");
        ret = wiz_sock_send(sn, buf, buf_size);
        if (ret < 0) {
          wiz_sock_close(sn);
          return ret;
        }
      } else if (opt == RECV_DATA) {
        uint32_t size = 0;
        if ((size = getSn_RX_RSR(sn)) > 0) {
          if (size > buf_size) size = buf_size;
          memset(buf,0,buf_size);
          ret = wiz_sock_recv(sn, buf, size);
          printf("Recv message: %s\n",buf);
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

void TestSocketAsServer(int argc, char *argv[])
{
    x_err_t ret;
    uint8_t buf[BUFF_SIZE] = {0};

    while (1) {
        ret = wiz_server_op(0, buf, BUFF_SIZE, TCP_PORT, RECV_DATA);
        if (ret > 0) {
        wiz_server_op(0, buf, BUFF_SIZE, TCP_PORT, SEND_DATA);
        };
    }

    return ;
    
}

PRIV_SHELL_CMD_FUNCTION(TestSocketAsServer, a w5500 server test sample, PRIV_SHELL_CMD_MAIN_ATTR);

#elif defined ETHERNET_AS_CLIENT

static uint32_t wiz_client_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
                       uint8_t dst_ip[4], uint16_t dst_port,
                       enum TCP_OPTION opt) {
  // assert(buf_size <= g_wiznet_buf_size);
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


void TestSocketAsClient(int argc, char *argv[])
{
    x_err_t ret;
    uint8_t buf[BUFF_SIZE] = {0};
    uint32_t tmp_ip[4];
    uint32_t port;
    const uint8_t client_sock = 2;

    if(argc<3){
      printf("Please enter command like TestSocketAsClient ip:port msg\n");
    }

    sscanf(argv[1],"%d.%d.%d.%d:%d",tmp_ip,tmp_ip+1,tmp_ip+2,tmp_ip+3,&port);
    printf("Client to %d.%d.%d.%d:%d\n",tmp_ip[0],tmp_ip[1],tmp_ip[2],tmp_ip[3],port);
    uint8_t destination_ip[4]={0};
    for(int i=0;i<4;i++){
      destination_ip[i]=tmp_ip[i];
    }
    
    while(1){
      ret = wiz_client_op(client_sock, argv[2], strlen(argv[2]), destination_ip, port,SEND_DATA);
      printf("sizeof:%d\n",strlen(argv[2]));
      PrivTaskDelay(1000); 
      if (ret > 0) {
        ret=wiz_client_op(client_sock, buf, BUFF_SIZE, destination_ip, port, RECV_DATA);
        printf("read ret is %d\n",ret);
        if(ret>0){
          printf("client recv msg successfully!\n");
          printf("%s\n",buf);            
        }
      };
      PrivTaskDelay(1000);      
    }


    

    return ;
    
}

PRIV_SHELL_CMD_FUNCTION(TestSocketAsClient, a w5500 client-ip-port-msg test sample, PRIV_SHELL_CMD_MAIN_ATTR);


#endif


#endif