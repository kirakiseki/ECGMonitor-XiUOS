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
* @file k210_w5500.h
* @brief w5500 driver
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-9-15
*/

#ifndef _K210_W5500_H_
#define _K210_W5500_H_

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <arch/board/board.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>
#include <assert.h>
#include <nuttx/time.h>
#include <nuttx/fs/fs.h>
#include "k210_config.h"
#include "k210_fpioa.h"
#include "k210_gpiohs.h"
#include "nuttx/arch.h"
#include "k210_gpio_common.h"

/***************** Common Register *****************/
#define W5500_MR_REG 0x0000
#define MR_RST 0x80
#define MR_WOL 0x20
#define MR_PB 0x10
#define MR_PPP 0x08
#define MR_FARP 0x02

#define W5500_GAR_REG 0x0001
#define W5500_SUBR_REG 0x0005
#define W5500_SHAR_REG 0x0009
#define W5500_SIPR_REG 0x000f

#define W5500_INT_REG 0x0013

#define W5500_IR_REG 0x0015
#define IR_CONFLICT 0x80
#define IR_UNREACH 0x40
#define IR_PPPOE 0x20
#define IR_MP  0x10

#define W5500_IMR_REG 0x0016
#define IMR_IR7 0x80
#define IMR_IR6 0x40
#define IMR_IR5 0x20
#define IMR_IR4 0x10

#define W5500_SIR_REG 0x0017
#define S7_INT 0x80
#define S6_INT 0x40
#define S5_INT 0x20
#define S4_INT 0x10
#define S3_INT 0x08
#define S2_INT 0x04
#define S1_INT 0x02
#define S0_INT 0x01

#define W5500_SIMR_REG 0x0018
#define S7_IMR 0x80
#define S6_IMR 0x40
#define S5_IMR 0x20
#define S4_IMR 0x10
#define S3_IMR 0x08
#define S2_IMR 0x04
#define S1_IMR 0x02
#define S0_IMR 0x01

#define W5500_RTR_REG 0x0019
#define W5500_RCR_REG 0x001B

#define W5500_PTIMER_REG 0x001C
#define W5500_PMAGIC_REG 0x001D
#define W5500_PHA_REG 0x001E
#define W5500_PSID_REG 0x0024
#define W5500_PMRU_REG 0x0026

#define W5500_UIPR_REG 0x0028
#define W5500_UPORT_REG 0x002C

#define W5500_PHYCFGR_REG 0x002E
#define RST_PHY 0x80
#define OPMODE 0x40
#define DPX  0x04
#define SPD  0x02
#define LINK 0x01

#define W5500_VER_REG 0x0039

/********************* Socket Register *******************/
#define W5500_SN_MR_REG 0x0000
#define SN_MR_MULTI_MFEN 0x80
#define SN_MR_BCASTB  0x40
#define SN_MR_ND_MC_MMB 0x20
#define SN_MR_UCASTB_MIP6B 0x10
#define SN_MR_CLOSE 0x00
#define SN_MR_TCP 0x01
#define SN_MR_UDP 0x02
#define SN_MR_MACRAW 0x04

#define W5500_SN_CR_REG 0x0001
#define SN_CR_OPEN 0x01
#define SN_CR_LISTEN 0x02
#define SN_CR_CONNECT 0x04
#define SN_CR_DISCON 0x08
#define SN_CR_CLOSE 0x10
#define SN_CR_SEND 0x20
#define SN_CR_SEND_MAC 0x21
#define SN_CR_SEND_KEEP 0x22
#define SN_CR_RECV 0x40

#define W5500_SN_IR_REG 0x0002
#define IR_SEND_OK 0x10
#define IR_TIMEOUT 0x08
#define IR_RECV  0x04
#define IR_DISCON 0x02
#define IR_CON  0x01

#define W5500_SN_SR_REG 0x0003
#define SOCK_CLOSED 0x00
#define SOCK_INIT 0x13
#define SOCK_LISTEN 0x14
#define SOCK_ESTABLISHED 0x17
#define SOCK_CLOSE_WAIT 0x1C
#define SOCK_UDP 0x22
#define SOCK_MACRAW 0x02

#define SOCK_SYNSEND 0x15
#define SOCK_SYNRECV 0x16
#define SOCK_FIN_WAI 0x18
#define SOCK_CLOSING 0x1A
#define SOCK_TIME_WAIT 0x1B
#define SOCK_LAST_ACK 0x1D

#define W5500_SN_PORT_REG 0x0004
#define W5500_SN_DHAR_REG 0x0006
#define W5500_SN_DIPR_REG 0x000C
#define W5500_SN_DPORTR_REG 0x0010

#define W5500_SN_MSSR_REG 0x0012
#define W5500_SN_TOS_REG 0x0015
#define W5500_SN_TTL_REG 0x0016

#define W5500_SN_RXBUF_SIZE_REG 0x001E
#define W5500_SN_TXBUF_SIZE_REG 0x001F
#define W5500_SN_TX_FSR_REG 0x0020
#define W5500_SN_TX_RD_REG 0x0022
#define W5500_SN_TX_WR_REG 0x0024
#define W5500_SN_RX_RSR_REG 0x0026
#define W5500_SN_RX_RD_REG 0x0028
#define W5500_SN_RX_WR_REG 0x002A

#define W5500_SN_IMR_REG 0x002C
#define IMR_SENDOK 0x10
#define IMR_TIMEOUT 0x08
#define IMR_RECV 0x04
#define IMR_DISCON 0x02
#define IMR_CON 0x01

#define W5500_SN_FRAG_REG 0x002D
#define W5500_SN_KPALVTR_REG 0x002F

/************************ SPI Control Data *************************/

/* Operation mode bits */

#define VDM 0x00
#define FDM1 0x01
#define FDM2 0x02
#define FDM4 0x03

/* Read_Write control bit */
#define RWB_READ 0x00
#define RWB_WRITE 0x04

/* Block select bits */
#define COMMON_R 0x00

/* Socket 0 */
#define S0_REG 0x08
#define S0_TX_BUF 0x10
#define S0_RX_BUF 0x18

/* Socket 1 */
#define S1_REG 0x28
#define S1_TX_BUF 0x30
#define S1_RX_BUF 0x38

/* Socket 2 */
#define S2_REG 0x48
#define S2_TX_BUF 0x50
#define S2_RX_BUF 0x58

/* Socket 3 */
#define S3_REG 0x68
#define S3_TX_BUF 0x70
#define S3_RX_BUF 0x78

/* Socket 4 */
#define S4_REG 0x88
#define S4_TX_BUF 0x90
#define S4_RX_BUF 0x98

/* Socket 5 */
#define S5_REG 0xa8
#define S5_TX_BUF 0xb0
#define S5_RX_BUF 0xb8

/* Socket 6 */
#define S6_REG 0xc8
#define S6_TX_BUF 0xd0
#define S6_RX_BUF 0xd8

/* Socket 7 */
#define S7_REG 0xe8
#define S7_TX_BUF 0xf0
#define S7_RX_BUF 0xf8

// socket receive buffer size based on RMSR
#define SOCK_RECV_SIZE 2048
// socket send buffer size based on RMSR
#define SOCK_SEND_SIZE 2048

#define W5500_IP_ADDR_LEN 4
#define W5500_IP_MASK_LEN 4
#define W5500_GW_ADDR_LEN 4
#define W5500_MAC_ADDR_LEN 6

//for every socket

// socket mode
#define SOCK_TCP_SVR 0 //server mode
#define SOCK_TCP_CLI 1 //client mode
#define SOCK_UDP_MOD 2 //udp mode

// socket flag
#define SOCK_FLAG_INIT 1
#define SOCK_FLAG_CONN 2

// socket data state
#define SOCK_STAT_RECV 1
#define SOCK_STAT_SEND 2

typedef struct w5500_socket_s
{
    uint16_t local_port;
    uint8_t dst_ip[W5500_IP_ADDR_LEN];
    uint16_t dst_port;
    uint8_t mode; // 0: TCP Server; 1: TCP client; 2: UDP
    uint8_t flag; // 1: init ok; 2: connected
    uint8_t state; // 1: receive one; 2: send ok
}w5500_socket_t;

typedef struct
{
    uint8_t ip_addr[W5500_IP_ADDR_LEN];
    uint8_t ip_mask[W5500_IP_MASK_LEN];
    uint8_t gw_addr[W5500_GW_ADDR_LEN];
    uint8_t mac_addr[W5500_MAC_ADDR_LEN];
    uint8_t udp_ip[4];
    uint16_t udp_port;
    w5500_socket_t sock;
}w5500_param_t;


#define W5500_MAX_PACK_SIZE 1460
typedef unsigned char socket_t;

#define NCS_L() k210_gpiohs_set_value(FPIOA_ENET_NCS, GPIO_PV_LOW); up_udelay(1);
#define NCS_H() k210_gpiohs_set_value(FPIOA_ENET_NCS, GPIO_PV_HIGH); up_udelay(1);
#define SCLK_L() k210_gpiohs_set_value(FPIOA_ENET_SCLK, GPIO_PV_LOW); up_udelay(1);
#define SCLK_H() k210_gpiohs_set_value(FPIOA_ENET_SCLK, GPIO_PV_HIGH); up_udelay(1);
#define MOSI_L() k210_gpiohs_set_value(FPIOA_ENET_MOSI, GPIO_PV_LOW); up_udelay(1);
#define MOSI_H() k210_gpiohs_set_value(FPIOA_ENET_MOSI, GPIO_PV_HIGH); up_udelay(1);
#define RST_L() k210_gpiohs_set_value(FPIOA_ENET_NRST, GPIO_PV_LOW); up_mdelay(200);
#define RST_H() k210_gpiohs_set_value(FPIOA_ENET_NRST, GPIO_PV_HIGH); up_mdelay(200);

void w5500_write_sock_byte(socket_t sock, uint16_t reg, uint8_t dat);
void w5500_write_sock_short(socket_t sock, uint16_t reg, uint16_t dat);
void w5500_write_sock_long(socket_t sock, uint16_t reg, uint8_t *dat);
uint8_t w5500_read_byte(uint16_t reg);
uint8_t w5500_read_sock_byte(socket_t sock, uint16_t reg);
uint16_t w5500_read_sock_short(socket_t sock, uint16_t reg);
void w5500_write_sock_bytes(socket_t sock, uint8_t *dat, uint16_t size);
void w5500_reset(void);
void w5500_config_init(void);
uint8_t w5500_detect_gateway(void);
void w5500_socket_init(socket_t sock);
uint8_t w5500_socket_connect(socket_t sock);
uint8_t w5500_socket_listen(socket_t sock);
uint8_t w5500_socket_set_udp(socket_t sock);
void w5500_irq_process(void);
void w5500_intialization(void);
void w5500_load_param(void);
void w5500_socket_config(void);
uint16_t Process_Socket_Data(socket_t sock);
void SPI_Configuration(void);
void w5500_test(void);

#endif
