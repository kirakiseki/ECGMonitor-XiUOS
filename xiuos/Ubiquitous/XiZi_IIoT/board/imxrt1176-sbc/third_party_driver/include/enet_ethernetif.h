/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ENET_ETHERNETIF_H
#define ENET_ETHERNETIF_H

#include "lwip/err.h"
#include "lwip/netif.h"
#include "fsl_phy.h"
#include <board.h>
#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
#include "fsl_phyksz8081.h"
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT
#include "fsl_phyksz9131rnx.h"
#endif
#include "fsl_enet_mdio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef BOARD_NETWORK_USE_100M_ENET_PORT
/* Address of PHY interface. */
#define EXAMPLE_ENET0_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS
/* PHY operations. */
#define EXAMPLE_ENET0_PHY_OPS phyksz8081_ops
/* ENET instance select. */
#define NETIF_ENET0_INIT_FUNC ethernetif0_init
#endif

#ifdef BOARD_NETWORK_USE_1G_ENET_PORT
/* Address of PHY interface. */
#define EXAMPLE_ENET1_PHY_ADDRESS   BOARD_ENET1_PHY_ADDRESS
/* PHY operations. */
#define EXAMPLE_ENET1_PHY_OPS       phyksz9131rnx_ops
/* ENET instance select. */
#define NETIF_ENET1_INIT_FUNC ethernetif1_init
#endif

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops

/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetRootClockFreq(kCLOCK_Root_Bus)

#define ENET_TIMEOUT        (0xFFFU)

/* ENET IRQ priority. Used in FreeRTOS. */
/* Interrupt priorities. */
#ifdef __CA7_REV
#ifndef ENET_PRIORITY
    #define ENET_PRIORITY       (21U)
#endif
#ifndef ENET_1588_PRIORITY
    #define ENET_1588_PRIORITY  (20U)
#endif
#else
#ifndef ENET_PRIORITY
    #define ENET_PRIORITY       (6U)
#endif
#ifndef ENET_1588_PRIORITY
    #define ENET_1588_PRIORITY  (5U)
#endif
#endif

/*  Defines Ethernet Autonegotiation Timeout during initialization. 
 *  Set it to 0 to disable the waiting. */ 
#ifndef ENET_ATONEGOTIATION_TIMEOUT
    #define ENET_ATONEGOTIATION_TIMEOUT     (0x2FFFFU)
#endif

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define ENET_RING_NUM 1U

#if (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0))
typedef struct mem_range
{
    uint32_t start;
    uint32_t end;
} mem_range_t;
#endif /* FSL_FEATURE_SOC_LPC_ENET_COUNT */

/**
 * Helper struct to hold data for configuration of ethernet interface.
 */
typedef struct ethernetif_config
{
    phy_handle_t *phyHandle;
    uint8_t macAddress[NETIF_MAX_HWADDR_LEN];
#if (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0))
    const mem_range_t *non_dma_memory;
#endif /* FSL_FEATURE_SOC_LPC_ENET_COUNT */
} ethernetif_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the first ENET interface.
 */
err_t ethernetif0_init(struct netif *netif);

#if (defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 1)) \
 || (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 1))
/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the second ENET interface.
 */
err_t ethernetif1_init(struct netif *netif);
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */

/**
 * This function should be called when a packet is ready to be read
 * from the interface. 
 * It is used by bare-metal applications.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input( void *netif_arg);

int ETH_BSP_Config(void);
void *ethernetif_config_enet_set(uint8_t enet_port);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* ENET_ETHERNETIF_H */
