/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"

#if USE_RTOS && defined(SDK_OS_FREE_RTOS)
#include "FreeRTOS.h"
#include "event_groups.h"
#endif

#include "pin_mux.h"
#include "enet_ethernetif.h"
#include "enet_ethernetif_priv.h"
#include "sys_arch.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};

#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
static phy_handle_t enet0_phyHandle   = {.phyAddr = EXAMPLE_ENET0_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_ENET0_PHY_OPS};
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT
static phy_handle_t enet1_phyHandle   = {.phyAddr = EXAMPLE_ENET1_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_ENET1_PHY_OPS};
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void Time_Update_LwIP(void)
{
}
#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
ethernetif_config_t enet0_cfg = {
    .phyHandle = &enet0_phyHandle,
    .macAddress = configMAC_ADDR,
};
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT
ethernetif_config_t enet1_cfg = {
    .phyHandle = &enet1_phyHandle,
    .macAddress = configMAC_ADDR_ETH1,
};
#endif

void *ethernetif_config_enet_set(uint8_t enet_port)
{

#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
    if(0 == enet_port) {
        KPrintf("use 100M enet id[%d]\n",enet_port);
        return (void *)&enet0_cfg;
    }
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT
    if(1 == enet_port) {
        KPrintf("use 1000M enet id[%d]\n",enet_port);
        return (void *)&enet1_cfg;
    }
        
#endif
        return NULL;
}

int ETH_BSP_Config(void)
{
    gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
    
    GPIO_PinInit(GPIO9, 11, &gpio_config);
    GPIO_PinInit(GPIO8, 21, &gpio_config);
    /* Pull up the ENET_INT before RESET. */
    GPIO_WritePinOutput(GPIO9, 11, 1);
    GPIO_WritePinOutput(GPIO8, 21, 0);
    SDK_DelayAtLeastUs(10000, CLOCK_GetFreq(kCLOCK_CpuClk));
    GPIO_WritePinOutput(GPIO8, 21, 1);
    SDK_DelayAtLeastUs(6, CLOCK_GetFreq(kCLOCK_CpuClk));
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT

    EnableIRQ(ENET_1G_MAC0_Tx_Rx_1_IRQn);
    EnableIRQ(ENET_1G_MAC0_Tx_Rx_2_IRQn);
#endif
    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;
    return 0;
}
void ethernetif_phy_init(struct ethernetif *ethernetif,
                         const ethernetif_config_t *ethernetifConfig,
                         phy_speed_t *speed,
                         phy_duplex_t *duplex)
{
    status_t status;
    bool link              = false;
    bool autonego          = false;
    uint32_t initWaitCount = 0;
    uint32_t autoWaitCount = 0;
    phy_config_t phyConfig = {
        .phyAddr = ethernetifConfig->phyHandle->phyAddr,
        .autoNeg = true,
    };

    ethernetifConfig->phyHandle->mdioHandle->resource.base = *ethernetif_enet_ptr(ethernetif);

    LWIP_PLATFORM_DIAG(("Initializing PHY..."));

    while ((initWaitCount < ENET_ATONEGOTIATION_TIMEOUT) && (!(link && autonego)))
    {
        status = PHY_Init(ethernetifConfig->phyHandle, &phyConfig);


        if (kStatus_Success != status)
        {
            LWIP_ASSERT("\r\nCannot initialize PHY.\r\n", 0);
        }

        /* Wait for auto-negotiation success and link up */
        autoWaitCount = ENET_ATONEGOTIATION_TIMEOUT;
        do
        {
            PHY_GetAutoNegotiationStatus(ethernetifConfig->phyHandle, &autonego);
            PHY_GetLinkStatus(ethernetifConfig->phyHandle, &link);
            if (autonego && link)
            {
                break;
            }
        } while (--autoWaitCount);
        if (!autonego)
        {
            LWIP_PLATFORM_DIAG(("PHY Auto-negotiation failed. Please check the cable connection and link partner setting."));
        }

        initWaitCount++;
    }

    if (autonego && link)
    {
        /* Get the actual PHY link speed. */
        PHY_GetLinkSpeedDuplex(ethernetifConfig->phyHandle, speed, duplex);
    }
#if 0 /* Disable assert. If initial auto-negation is timeout, \ \
         the ENET is set to default (100Mbs and full-duplex). */
    else
    {
        LWIP_ASSERT("\r\nGiving up PHY initialization. Please check the ENET cable connection and link partner setting and reset the board.\r\n", 0);
    }
#endif
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function ethernetif_linkinput() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(void *netif_arg)
{
    struct pbuf *p;
    struct netif *netif = (struct netif *)netif_arg;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /* move received packet into a new pbuf */
    while ((p = ethernetif_linkinput(netif)) != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

void *ethernetif_get_enet_base(const uint8_t enetIdx)
{
    ENET_Type *enets[] = ENET_BASE_PTRS;
    int arrayIdx;
    int enetCount;

    for (arrayIdx = 0, enetCount = 0; arrayIdx < ARRAY_SIZE(enets); arrayIdx++)
    {
        if (enets[arrayIdx] != 0U) /* process only defined positions */
        {                          /* (some SOC headers count ENETs from 1 instead of 0) */
            if (enetCount == enetIdx)
            {
                return (void *)enets[arrayIdx];
            }
            enetCount++;
        }
    }

    return NULL;
}

#if defined(FSL_FEATURE_SOC_ENET_QOS_COUNT) && (FSL_FEATURE_SOC_ENET_QOS_COUNT > 0)
void *ethernetif_get_enet_qos_base(const uint8_t enetIdx)
{
    ENET_QOS_Type *enets[] = ENET_QOS_BASE_PTRS;
    int arrayIdx;
    int enetCount;

    for (arrayIdx = 0, enetCount = 0; arrayIdx < ARRAY_SIZE(enets); arrayIdx++)
    {
        if (enets[arrayIdx] != 0U) /* process only defined positions */
        {                          /* (some SOC headers count ENETs from 1 instead of 0) */
            if (enetCount == enetIdx)
            {
                return (void *)enets[arrayIdx];
            }
            enetCount++;
        }
    }

    return NULL;
}
#endif

err_t ethernetif_init(struct netif *netif,
                      struct ethernetif *ethernetif,
                      void *enetBase,
                      const ethernetif_config_t *ethernetifConfig)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("ethernetifConfig != NULL", (ethernetifConfig != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state   = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
/* We directly use etharp_output() here to save a function call.
 * You can instead declare your own function an call etharp_output()
 * from it if you have to do some checks before sending (e.g. if link
 * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = ethernetif_linkoutput;

#if LWIP_IPV4 && LWIP_IGMP
    netif_set_igmp_mac_filter(netif, ethernetif_igmp_mac_filter);
    netif->flags |= NETIF_FLAG_IGMP;
#endif
#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif_set_mld_mac_filter(netif, ethernetif_mld_mac_filter);
    netif->flags |= NETIF_FLAG_MLD6;
#endif

    /* Init ethernetif parameters.*/
    *ethernetif_enet_ptr(ethernetif) = enetBase;
    LWIP_ASSERT("*ethernetif_enet_ptr(ethernetif) != NULL", (*ethernetif_enet_ptr(ethernetif) != NULL));

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, ethernetifConfig->macAddress, NETIF_MAX_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500; /* TODO: define a config */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* ENET driver initialization.*/
    ethernetif_enet_init(netif, ethernetif, ethernetifConfig);

#if LWIP_IPV6 && LWIP_IPV6_MLD
    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL)
    {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

    return ERR_OK;
}
