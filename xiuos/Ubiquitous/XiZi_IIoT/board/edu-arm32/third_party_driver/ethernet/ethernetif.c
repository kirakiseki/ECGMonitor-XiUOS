/**
 *******************************************************************************
 * @file  eth/eth_loopback/source/ethernetif.c
 * @brief This file implements Ethernet network interface drivers.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/**
* @file ethernetif.c
* @brief support edu-arm32-board ethernetif function and register to Lwip
* @version 3.0 
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

/*************************************************
File name: ethernetif.c
Description: support edu-arm32-board ethernetif configure and register to Lwip
Others: take projects\ev_hc32f4a0_lqfp176\examples\eth\eth_loopback\source\ethernetif.c for references
History: 
1. Date: 2022-12-05
Author: AIIT XUOS Lab
Modification: 
1、include harware_ethernetif.h、hc32_ll_eth.h、hc32_ll_gpio.h、hc32_ll_utility.h、hc32_ll_fcg.h and lwip H files;
2、modify ethernetif_init as err_t;
3、add ETH_RST_PORT and ETH_RST_PIN;
4、add ETH_LINK_LED_PORT and ETH_LINK_LED_PIN;
5、add ethernetif_config_enet_set;
6、add ETHERNET_LOOPBACK_TEST with testnetif and txPbuf;
7、modify ethernetif_init() and ethernetif_input() to support LwIP.
*************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <connect_ethernet.h>
#include <hc32_ll_gpio.h>
#include <hc32_ll_utility.h>
#include <hc32_ll_fcg.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>

#include <xs_isr.h>

/**
 * @addtogroup HC32F4A0_DDL_Examples
 * @{
 */

/**
 * @addtogroup ETH_Loopback
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/


/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @defgroup ETH_IF_Global_Functions Ethernet Interface Global Functions
 * @{
 */

/**
 * @brief  Initializes the Ethernet GPIO.
 * @param  None
 * @retval None
 */
void Ethernet_GpioInit(void)
{
    /* ETH_RST */
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(ETH_RST_PORT, ETH_RST_PIN, &stcGpioInit);
    GPIO_ResetPins(ETH_RST_PORT, ETH_RST_PIN);

    SysTick_Delay(PHY_HW_RST_DELAY);
    GPIO_SetPins(ETH_RST_PORT, ETH_RST_PIN);
    SysTick_Delay(PHY_HW_RST_DELAY);

    /* ETH_LINK_LED LED2 */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(ETH_LINK_LED_PORT, ETH_LINK_LED_PIN, &stcGpioInit);
    GPIO_ResetPins(ETH_LINK_LED_PORT, ETH_LINK_LED_PIN);

    /* Configure MII/RMII selection IO for ETH */
#ifdef ETH_INTERFACE_RMII
    /* Ethernet RMII pins configuration */
    /*
        ETH_SMI_MDIO ----------------> PA2
        ETH_SMI_MDC -----------------> PC1
        ETH_RMII_TX_EN --------------> PG11
        ETH_RMII_TXD0 ---------------> PG13
        ETH_RMII_TXD1 ---------------> PG14
        ETH_RMII_REF_CLK ------------> PA1
        ETH_RMII_CRS_DV -------------> PA7
        ETH_RMII_RXD0 ---------------> PC4
        ETH_RMII_RXD1 ---------------> PC5
        ETH_RMII_RX_ER --------------> PI10
    */
    /* Configure PA1, PA2 and PA7 */
    GPIO_SetFunc(GPIO_PORT_A, (GPIO_PIN_01 | GPIO_PIN_02 | GPIO_PIN_07), GPIO_FUNC_11);
    /* Configure PC1, PC4 and PC5 */
    GPIO_SetFunc(GPIO_PORT_C, (GPIO_PIN_01 | GPIO_PIN_04 | GPIO_PIN_05), GPIO_FUNC_11);
    /* Configure PG11, PG13 and PG14 */
    GPIO_SetFunc(GPIO_PORT_G, (GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14), GPIO_FUNC_11);
    /* Configure PI10 */
    GPIO_SetFunc(GPIO_PORT_I, GPIO_PIN_10, GPIO_FUNC_11);
#else
    /* Ethernet MII pins configuration */
    /*
        ETH_SMI_MDIO ----------------> PA2
        ETH_SMI_MDC -----------------> PC1
        ETH_MII_TX_CLK --------------> PB6
        ETH_MII_TX_EN ---------------> PG11
        ETH_MII_TXD0 ----------------> PG13
        ETH_MII_TXD1 ----------------> PG14
        ETH_MII_TXD2 ----------------> PB9
        ETH_MII_TXD3 ----------------> PB8
        ETH_MII_RX_CLK --------------> PA1
        ETH_MII_RX_DV ---------------> PA7
        ETH_MII_RXD0 ----------------> PC4
        ETH_MII_RXD1 ----------------> PC5
        ETH_MII_RXD2 ----------------> PB0
        ETH_MII_RXD3 ----------------> PB1
        ETH_MII_RX_ER ---------------> PI10
        ETH_MII_CRS -----------------> PH2
        ETH_MII_COL -----------------> PH3
    */
    /* Configure PA1, PA2 and PA7 */
    GPIO_SetFunc(GPIO_PORT_A, (GPIO_PIN_01 | GPIO_PIN_02 | GPIO_PIN_07), GPIO_FUNC_11);
    /* Configure PB0, PB1, PB6, PB8 and PB9 */
    GPIO_SetFunc(GPIO_PORT_B, (GPIO_PIN_00 | GPIO_PIN_01 | GPIO_PIN_06 | GPIO_PIN_08 | GPIO_PIN_09), GPIO_FUNC_11);
    /* Configure PC1, PC4 and PC5 */
    GPIO_SetFunc(GPIO_PORT_C, (GPIO_PIN_01 | GPIO_PIN_04 | GPIO_PIN_05), GPIO_FUNC_11);
    /* Configure PG11, PG13 and PG14 */
    GPIO_SetFunc(GPIO_PORT_G, (GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14), GPIO_FUNC_11);
    /* Configure PH2, PH3 */
    GPIO_SetFunc(GPIO_PORT_H, (GPIO_PIN_02 | GPIO_PIN_03), GPIO_FUNC_11);
    /* Configure PI10 */
    GPIO_SetFunc(GPIO_PORT_I, GPIO_PIN_10, GPIO_FUNC_11);
#endif
}

void *ethernetif_config_enet_set(uint8_t enet_port) {
    return NONE;
}

void Time_Update_LwIP(void) {
    //no need to do
}

/**
 * @brief  Should be called at the beginning of the program to set up the network interface.
 * @param  netif                        The network interface structure for this ethernetif.
 * @retval err_t:
 *           - LL_OK: The IF is initialized
 *           - LL_ERR: The IF is uninitialized
 */
err_t ethernetif_init(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

#ifndef ETHERNET_LOOPBACK_TEST
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;
#endif

    /* initialize the hardware */
    return low_level_init(netif);
}

/**
 * @brief  This function should be called when a packet is ready to be read from the interface.
 * @param  netif                        The network interface structure for this ethernetif.
 * @retval None
 */
void ethernetif_input(void *netif_arg)
{
    struct pbuf *p;
    struct netif *netif = (struct netif *)netif_arg;
    x_base critical_lock;

    /* Move received packet into a new pbuf */
    while (1) {
        sys_arch_sem_wait(get_eth_recv_sem(), WAITING_FOREVER);
        while(1) {
            p = low_level_input(netif);
#ifndef ETHERNET_LOOPBACK_TEST
            /* Entry point to the LwIP stack */
            if (p != NULL) {
                if (netif->input(p, netif) != ERR_OK) {
                    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                    KPrintf("ethernetif_input: IP input error\n");
                    pbuf_free(p);
                    p = NULL;
                }
            } else {
                break;
            }
#else
            /* No packet could be read, silently ignore this */
            if (p != NULL) {
                EthernetIF_InputCallback(netif, p);
                free(p);
            }
#endif
        }
    }
}

/**
 * @brief  Check the netif link status.
 * @param  netif the network interface
 * @retval None
 */
void EthernetIF_CheckLink(struct netif *netif)
{
    uint16_t u16RegVal = 0U;
    static uint8_t u8PreStatus = 0U;

    if (1U == u8EthInitStatus) {
        u8EthInitStatus = 0U;
        u8PhyLinkStatus = ETH_LINK_UP;
        u8PreStatus = 1U;
        /* Notify link status change */
        EthernetIF_NotifyLinkChange(netif);
    } else {
        /* Read PHY_BSR */
        (void)ETH_PHY_ReadReg(&EthHandle, PHY_BSR, &u16RegVal);
        /* Check whether the link is up or down*/
        if ((0x0000U != u16RegVal) && (0xFFFFU != u16RegVal)) {
            if ((0U != (u16RegVal & PHY_LINK_STATUS)) && (0U == u8PreStatus)) {
                u8PhyLinkStatus = ETH_LINK_UP;
                u8PreStatus = 1U;
                EthernetIF_LinkCallback(netif);
            }
            if ((0U == (u16RegVal & PHY_LINK_STATUS)) && (1U == u8PreStatus)) {
                u8PhyLinkStatus = ETH_LINK_DOWN;
                u8PreStatus = 0U;
                EthernetIF_LinkCallback(netif);
            }
        }
    }
}

/**
 * @brief  Update the netif link status.
 * @param  netif                        The network interface.
 * @retval None
 */
void EthernetIF_UpdateLink(struct netif *netif)
{
    uint16_t u16RegVal;

    if (1U == u8EthInitStatus) {
        u8EthInitStatus = 0U;
        u8PhyLinkStatus = ETH_LINK_UP;
        /* Notify link status change */
        EthernetIF_NotifyLinkChange(netif);
    } else {
        u16RegVal = PHY_PAGE_ADDR_0;
        (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSR, u16RegVal);
        /* Read PHY_IISDR */
        (void)ETH_PHY_ReadReg(&EthHandle, PHY_IISDR, &u16RegVal);
        /* Check whether the link interrupt has occurred or not */
        if (0U != (u16RegVal & PHY_FLAG_LINK_STATUS_CHANGE)) {
            /* Read PHY_BSR */
            (void)ETH_PHY_ReadReg(&EthHandle, PHY_BSR, &u16RegVal);
            if ((0x0000U != u16RegVal) && (0xFFFFU != u16RegVal)) {
                if (ETH_LINK_UP != u8PhyLinkStatus) {
                    /* Wait until the auto-negotiation will be completed */
                    SysTick_Delay(2U);
                    (void)ETH_PHY_ReadReg(&EthHandle, PHY_BSR, &u16RegVal);
                }
                /* Check whether the link is up or down*/
                if (0U != (u16RegVal & PHY_LINK_STATUS)) {
                    u8PhyLinkStatus = ETH_LINK_UP;
                } else {
                    u8PhyLinkStatus = ETH_LINK_DOWN;
                }
                EthernetIF_LinkCallback(netif);
            }
        }
    }
}

/**
 * @brief  Ethernet interface periodic handle
 * @param  netif                        The network interface
 * @retval None
 */
void EthernetIF_PeriodicHandle(struct netif *netif)
{
#ifndef ETH_INTERFACE_RMII
    uint32_t curTick;
    static uint32_t u32LinkTimer = 0UL;

    curTick = SysTick_GetTick();
    /* Check link status periodically */
    if ((curTick - u32LinkTimer) >= LINK_TIMER_INTERVAL) {
        u32LinkTimer = curTick;
        EthernetIF_CheckLink(netif);
    }
#endif /* ETH_INTERFACE_RMII */
}

/**
 * @brief  Link callback function
 * @note   This function is called on change of link status to update low level driver configuration.
 * @param  netif                        The network interface
 * @retval None
 */
void EthernetIF_LinkCallback(struct netif *netif)
{
    __IO uint32_t tickStart = 0UL;
    uint16_t u16RegVal = 0U;
    int32_t i32negoResult = LL_ERR;

    if (ETH_LINK_UP == u8PhyLinkStatus) {
        /* Restart the auto-negotiation */
        if (ETH_AUTO_NEGO_DISABLE != (EthHandle.stcCommInit).u16AutoNego) {
            /* Enable Auto-Negotiation */
            (void)ETH_PHY_ReadReg(&EthHandle, PHY_BCR, &u16RegVal);
            u16RegVal |= PHY_AUTONEGOTIATION;
            (void)ETH_PHY_WriteReg(&EthHandle, PHY_BCR, u16RegVal);

            /* Wait until the auto-negotiation will be completed */
            tickStart = SysTick_GetTick();
            do {
                (void)ETH_PHY_ReadReg(&EthHandle, PHY_BSR, &u16RegVal);
                if (PHY_AUTONEGO_COMPLETE == (u16RegVal & PHY_AUTONEGO_COMPLETE)) {
                    break;
                }
                /* Check for the Timeout (3s) */
            } while ((SysTick_GetTick() - tickStart) <= 3000U);
            if (PHY_AUTONEGO_COMPLETE == (u16RegVal & PHY_AUTONEGO_COMPLETE)) {
                i32negoResult = LL_OK;
                /* Configure ETH duplex mode according to the result of automatic negotiation */
                if (0U != (u16RegVal & (PHY_100BASE_TX_FD | PHY_10BASE_T_FD))) {
                    (EthHandle.stcCommInit).u32DuplexMode = ETH_MAC_DUPLEX_MD_FULL;
                } else {
                    (EthHandle.stcCommInit).u32DuplexMode = ETH_MAC_DUPLEX_MD_HALF;
                }

                /* Configure ETH speed according to the result of automatic negotiation */
                if (0U != (u16RegVal & (PHY_100BASE_TX_FD | PHY_100BASE_TX_HD))) {
                    (EthHandle.stcCommInit).u32Speed = ETH_MAC_SPEED_100M;
                } else {
                    (EthHandle.stcCommInit).u32Speed = ETH_MAC_SPEED_10M;
                }
            }
        }

        /* AutoNegotiation disable or failed*/
        if (LL_ERR == i32negoResult) {
            (void)ETH_PHY_ReadReg(&EthHandle, PHY_BCR, &u16RegVal);
            CLR_REG16_BIT(u16RegVal, PHY_FULLDUPLEX_100M);
            /* Set MAC Speed and Duplex Mode to PHY */
            (void)ETH_PHY_WriteReg(&EthHandle, PHY_BCR,
                                   ((uint16_t)((EthHandle.stcCommInit).u32DuplexMode >> 3U) |
                                    (uint16_t)((EthHandle.stcCommInit).u32Speed >> 1U) | u16RegVal));
        }
        /* ETH MAC Re-Configuration */
        ETH_MAC_SetDuplexSpeed((EthHandle.stcCommInit).u32DuplexMode, (EthHandle.stcCommInit).u32Speed);
        /* Restart MAC interface */
        (void)ETH_Start();
    } else {
        /* Stop MAC interface */
        (void)ETH_Stop();
    }
    /* Notify link status change */
    EthernetIF_NotifyLinkChange(netif);
}

/**
 * @brief  Ethernet interface periodic handle
 * @param  netif                        The network interface
 * @retval int32_t:
 *           - LL_OK: The IF is link up
 *           - LL_ERR: The IF is link down
 */
int32_t EthernetIF_IsLinkUp(struct netif *netif)
{
    return (0U != u8PhyLinkStatus) ? LL_OK : LL_ERR;
}

/**
 * @brief  Notify link status change.
 * @param  netif                        The network interface
 * @retval None
 */
__WEAKDEF void EthernetIF_NotifyLinkChange(struct netif *netif)
{
    /* This is function could be implemented in user file when the callback is needed */
    if (LL_OK == EthernetIF_IsLinkUp(netif)) {
        GPIO_SetPins(ETH_LINK_LED_PORT, ETH_LINK_LED_PIN);
    } else {
        GPIO_ResetPins(ETH_LINK_LED_PORT, ETH_LINK_LED_PIN);
    }    
}

/**
 * @brief  Input data handle callback.
 * @param  netif                        The network interface structure for this ethernetif
 * @param  p                            The MAC packet to receive
 * @retval None
 */
__WEAKDEF void EthernetIF_InputCallback(struct netif *netif, struct pbuf *p) {
    /* This is function could be implemented in user file when the callback is needed */
#ifdef ETHERNET_LOOPBACK_TEST
    if ((0 == (memcmp(p->payload, txPbuf.payload, p->len))) && (p->len == txPbuf.len)) {
        KPrintf("eth receive data OK! \r\n");
        KPrintf("receive data %d %s\n", p->len, p->payload);
    } else {
        KPrintf("eth receive data error! \r\n");
    }
#endif
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef ETHERNET_LOOPBACK_TEST

static void EthLoopBackTask(void *parameter)
{
    while (1) {
        if (RESET == GPIO_ReadInputPins(USER_KEY_PORT, USER_KEY_PIN)) {
            KPrintf("ready to send eth data\n");
            if (LL_OK != low_level_output(&testnetif, &txPbuf)) {
                KPrintf("eth send data error! \r\n");
            }
        }

        //KPrintf("ready to receive eth loop back data\n");
        /* Read a received packet */
        ethernetif_input(&testnetif);
        /* Handle periodic timers */
        EthernetIF_PeriodicHandle(&testnetif);
    }
}

static void EthLoopBackTest(void)
{
    x_err_t ret = EOK;

    stc_gpio_init_t stcGpioInit;

    /* KEY initialize */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(USER_KEY_PORT, USER_KEY_PIN, &stcGpioInit);
    GPIO_ResetPins(USER_KEY_PORT, USER_KEY_PIN);

    /* Configure the Ethernet */
    (void)ethernetif_init(&testnetif);

    /* fill data to txPbuf */
    txPbuf.next    = NULL;
    txPbuf.payload = txBuf;
    txPbuf.len     = strlen(txBuf);

    int eth_loopback_task = 0;
    eth_loopback_task = KTaskCreate("eth_loopback", EthLoopBackTask, NONE,
                           2048, 8);
    if(eth_loopback_task < 0) {		
		KPrintf("eth_loopback_task create failed ...%s %d.\n", __FUNCTION__,__LINE__);
		return;
	}

    StartupKTask(eth_loopback_task);

    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
EthLoopBackTest, EthLoopBackTest, EthLoopBackTest);

#endif

/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/
