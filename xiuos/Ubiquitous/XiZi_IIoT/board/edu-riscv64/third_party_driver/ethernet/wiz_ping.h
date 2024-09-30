#ifndef _WIZ_PING_H_
#define _WIZ_PING_H_

#include "socket.h"
#include "w5500.h"

#define PING_BUF_LEN 32
#define PING_REQUEST 8
#define PING_REPLY 0
#define CODE_ZERO 0

#define SOCKET_ERROR 1
#define TIMEOUT_ERROR 2
#define SUCCESS 3
#define REPLY_ERROR 4

typedef struct pingmsg {
  uint8_t Type;               // 0 - Ping Reply, 8 - Ping Request
  uint8_t Code;               // Always 0
  uint16_t CheckSum;          // Check sum
  uint16_t ID;                // Identification
  uint16_t SeqNum;            // Sequence Number
  int8_t Data[PING_BUF_LEN];  // Ping Data  : 1452 = IP RAW MTU -
                              // sizeof(Type+Code+CheckSum+ID+SeqNum)
} PINGMSGR;

uint8_t ping_count(uint8_t sn, uint16_t pCount, uint8_t *addr);

uint8_t ping_request(uint8_t s, uint8_t *addr);

uint8_t ping_reply(uint8_t s, uint8_t *addr, uint16_t rlen);

void Ethernet_ping_service_deal(uint8_t sn);

#endif