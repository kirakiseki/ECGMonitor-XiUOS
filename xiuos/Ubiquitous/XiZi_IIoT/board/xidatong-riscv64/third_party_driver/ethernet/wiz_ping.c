
#include "wiz_ping.h"

#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <xs_base.h>
#include <xs_ktask.h>

#define Sn_PROTO(ch) (0x001408 + (ch << 5))

#define PING_BIND_PORT 3000

PINGMSGR PingRequest = {0};
PINGMSGR PingReply = {0};

static uint16_t ping_RandomID = 0x1234;
static uint16_t ping_RandomSeqNum = 0x4321;
uint8_t ping_reply_received = 0;
uint8_t ping_req = 0;
uint8_t ping_rep = 0;
uint8_t ping_cnt = 0;
uint8_t ping_rep_buf[150] = {0};

// ping状态机
#define PING_STA_FREE 0
#define PING_STA_OPEN 1
#define PING_STA_SEND 2
#define PING_STA_WAIT 3
#define PING_STA_CLOSE 4

uint8_t ping_sta = PING_STA_FREE;

//当前ping的设备的序号
uint8_t ping_socket = 0;

#define bswap_16(A) ((((uint16)(A)&0xff00) >> 8) | (((uint16)(A)&0x00ff) << 8))

uint16_t htons(uint16_t n) {
  union {
    int i;
    char c;
  } u = {1};
  return u.c ? bswap_16(n) : n;
}

uint16_t checksum(uint8_t *src, uint32_t len) {
  uint16_t sum, tsum, i, j;
  uint32_t lsum;

  j = len >> 1;
  lsum = 0;

  for (i = 0; i < j; i++) {
    tsum = src[i * 2];
    tsum = tsum << 8;
    tsum += src[i * 2 + 1];
    lsum += tsum;
  }

  if (len % 2) {
    tsum = src[i * 2];
    lsum += (tsum << 8);
  }

  sum = lsum;
  sum = ~(sum + (lsum >> 16));
  return (uint16_t)sum;
}

/**
 *@brief		设定次数ping外网IP函数
 *@param		sn-   socket number
 *@param		addr-  外网IP地址
 *@param		pCount- ping的次数
 *@return	ping成功次数
 */
uint8_t ping_count(uint8_t sn, uint16_t pCount, uint8_t *addr) {
  uint16_t rlen, cnt, i;

  ping_reply_received = 0;
  ping_req = 0;
  ping_rep = 0;
  KPrintf("Ping:%d.%d.%d.%d\r\n", (addr[0]), (addr[1]), (addr[2]), (addr[3]));

  for (i = 0; i < pCount + 1; i++) /*循环ping pCount次*/
  {
    switch (getSn_SR(sn)) /*获取socket状态*/
    {
      case SOCK_CLOSED: /*socket关闭状态*/
      {
        wiz_sock_close(sn);
        /* Create Socket */
        IINCHIP_WRITE(Sn_PROTO(sn), IPPROTO_ICMP); /*设置ICMP 协议*/
        if (wiz_socket(sn, Sn_MR_IPRAW, PING_BIND_PORT, 0) !=
            0) /*判断ip raw模式socket是否开启*/
        {
        }
        /* Check socket register */
        while (getSn_SR(sn) != SOCK_IPRAW) {
          MdelayKTask(50);
        };
        break;
      }
      case SOCK_IPRAW: /*ip raw模式*/
      {
        cnt = 0;
        ping_request(sn, addr); /*发送Ping请求*/
        ping_req++;
        while (1) {
          if ((rlen = getSn_RX_RSR(sn)) > 0) {
            rlen = ping_reply(sn, addr, rlen); /*获取回复信息*/
            ping_rep++;
            if (ping_reply_received) {
              break;
            }
          }
          if ((cnt > 300)) {
            cnt = 0;
            break;
          } else {
            cnt++;
            MdelayKTask(10);
          }
        }
        break;
      }
      default:
        break;
    }
    if (ping_req >= pCount) {
      wiz_sock_close(sn);
    }
  }
  return ping_rep;
}

/**
 *@brief		ping请求函数
 *@param		sn-  socket number
 *@param		addr-  P地址
 *@return	无
 */
uint8_t ping_request(uint8_t sn, uint8_t *addr) {
  uint8_t *buffer;
  uint16_t i, temp_len = 0;
  ping_reply_received = 0;         /*ping 回复初始化标志位*/
  PingRequest.Type = PING_REQUEST; /*Ping-Request*/
  PingRequest.Code = CODE_ZERO;    /*总是 '0'*/
  PingRequest.ID = htons(ping_RandomID++); /*设置ping响应ID为随机的整型变量*/
  PingRequest.SeqNum =
      htons(ping_RandomSeqNum++); /*设置ping响应的序列号为随机整形变量*/
  for (i = 0; i < PING_BUF_LEN; i++) {
    PingRequest.Data[i] = (i) % 8; /*ping相应的数在'0'~'8‘*/
  }
  PingRequest.CheckSum = 0;
  /* 计算响应次数*/
  PingRequest.CheckSum =
      htons(checksum((uint8_t *)&PingRequest, sizeof(PingRequest)));

  /*发送ping响应到目的方 */
  if (wiz_sock_sendto(sn, (uint8_t *)&PingRequest, sizeof(PingRequest), addr,
                      PING_BIND_PORT) == 0) {
    KPrintf("Fail to send ping-reply packet\r\n");
  } else {
    KPrintf("ping send\n");
  }
  return 0;
}

/**
 *@brief		解析Ping回复
 *@param		sn-  socket number
 *@param		addr- Ping地址
 *@return	无
 */
uint8_t ping_reply(uint8_t sn, uint8_t *addr, uint16_t rlen) {
  uint16_t tmp_checksum;
  uint16_t len;
  uint16_t i;

  uint16_t port = PING_BIND_PORT;
  PINGMSGR PingReply;

  memset(ping_rep_buf, 0, sizeof(ping_rep_buf));
  len = wiz_sock_recvfrom(sn, ping_rep_buf, rlen, addr,
                          &port); /*从目的端接收数据*/

  if (ping_rep_buf[0] == PING_REPLY) {
    PingReply.Type = ping_rep_buf[0];
    PingReply.Code = ping_rep_buf[1];
    PingReply.CheckSum = (ping_rep_buf[3] << 8) + ping_rep_buf[2];
    PingReply.ID = (ping_rep_buf[5] << 8) + ping_rep_buf[4];
    PingReply.SeqNum = (ping_rep_buf[7] << 8) + ping_rep_buf[6];

    for (i = 0; i < len - 8; i++) {
      PingReply.Data[i] = ping_rep_buf[8 + i];
    }
    tmp_checksum = ~checksum(ping_rep_buf, len); /*检查ping回复的次数*/
    if (tmp_checksum != 0xffff) {
      KPrintf("tmp_checksum = %x\r\n", tmp_checksum);
    } else {
      KPrintf("Reply from %3d.%3d.%3d.%3d ID=%x Byte=%d\r\n\r\n", (addr[0]),
              (addr[1]), (addr[2]), (addr[3]), htons(PingReply.ID), (rlen + 6));
      ping_reply_received = 1; /*当退出ping回复循环时，设置ping回复标志为1*/
    }
  } else if (ping_rep_buf[0] == PING_REQUEST) {
    PingReply.Code = ping_rep_buf[1];
    PingReply.Type = ping_rep_buf[2];
    PingReply.CheckSum = (ping_rep_buf[3] << 8) + ping_rep_buf[2];
    PingReply.ID = (ping_rep_buf[5] << 8) + ping_rep_buf[4];
    PingReply.SeqNum = (ping_rep_buf[7] << 8) + ping_rep_buf[6];
    for (i = 0; i < len - 8; i++) {
      PingReply.Data[i] = ping_rep_buf[8 + i];
    }
    tmp_checksum = PingReply.CheckSum; /*检查ping回复次数*/
    PingReply.CheckSum = 0;
    if (tmp_checksum != PingReply.CheckSum) {
      KPrintf(" \n CheckSum is in correct %x shold be %x \n", (tmp_checksum),
              htons(PingReply.CheckSum));
    } else {
    }
    KPrintf(
        " Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
        (addr[0]), (addr[1]), (addr[2]), (addr[3]), (PingReply.ID),
        (PingReply.SeqNum), (rlen + 6));
    ping_reply_received = 1; /* 当退出ping回复循环时，设置ping回复标志为1
                              */
  } else {
    KPrintf(" Unkonwn msg. \n");
  }
  return 0;
}

void wiz_ping_test(int argc, char *argv[]) {
  uint32_t tmp_ip[4];
  uint8_t target_ip[4];
  if (argc >= 2) {
    KPrintf("This is a Ping test: %s\n", argv[1]);
    sscanf(argv[1], "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2],
           &tmp_ip[3]);
    target_ip[0] = (uint8_t)tmp_ip[0];
    target_ip[1] = (uint8_t)tmp_ip[1];
    target_ip[2] = (uint8_t)tmp_ip[2];
    target_ip[3] = (uint8_t)tmp_ip[3];
    ping_count(ping_socket, 5, target_ip);
  }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ping, wiz_ping_test, ping to given addr);