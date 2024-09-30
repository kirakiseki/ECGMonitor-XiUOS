/*
* Copyright (c) 2021 AIIT XUOS Lab
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
* @file connect_ethernet.h
* @brief Adapted network software protocol stack and hardware operation functions
* @version 2.0
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

#ifndef CONNECT_ETHERNET_H
#define CONNECT_ETHERNET_H

#include "hardware_ethernetif.h"
#include <sys_arch.h>
#include <hc32_ll_eth.h>
#include <hardware_irq.h>

#ifdef __cplusplus
 extern "C" {
#endif

struct hc32_irq_config
{
    IRQn_Type       irq_num;
    uint32_t        irq_prio;
    en_int_src_t    int_src;
};

/* Global Ethernet handle*/
static stc_eth_handle_t EthHandle;
/* Ethernet Tx DMA Descriptor */
__ALIGN_BEGIN static stc_eth_dma_desc_t EthDmaTxDscrTab[ETH_TX_BUF_NUM];
/* Ethernet Rx DMA Descriptor */
__ALIGN_BEGIN static stc_eth_dma_desc_t EthDmaRxDscrTab[ETH_RX_BUF_NUM];
/* Ethernet Transmit Buffer */
__ALIGN_BEGIN static uint8_t EthTxBuff[ETH_TX_BUF_NUM][ETH_TX_BUF_SIZE];
/* Ethernet Receive Buffer */
__ALIGN_BEGIN static uint8_t EthRxBuff[ETH_RX_BUF_NUM][ETH_RX_BUF_SIZE];

/* Ethernet link status */
static uint8_t u8PhyLinkStatus = 0U, u8EthInitStatus = 0U;

static struct Hc32IrqConfig eth_irq_config = {
    .irq_num    = BSP_ETH_IRQ_NUM,
    .irq_prio   = BSP_ETH_IRQ_PRIO,                     
    .int_src    = INT_SRC_ETH_GLB_INT, 
};

void Ethernet_GpioInit(void);
int32_t low_level_init(struct netif *netif);
err_t low_level_output(struct netif *netif, struct pbuf *p);
struct pbuf *low_level_input(struct netif *netif);

int HwEthInit(void);


#ifdef __cplusplus
}
#endif

#endif

