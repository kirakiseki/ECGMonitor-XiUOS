/**
* @file ethernetif.c
* @brief support edu-arm32-board ethernetif function and register to Lwip
* @version 3.0 
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

#include <connect_ethernet.h>
#include <hc32_ll_gpio.h>
#include <hc32_ll_utility.h>
#include <hc32_ll_fcg.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>

#include <sys_arch.h>

void eth_irq_handler(void) {
    static x_base eth_irq_lock;
    eth_irq_lock = DISABLE_INTERRUPT();

    // handle irq
    if (RESET != ETH_DMA_GetStatus(ETH_DMA_FLAG_RIS)) {
        sys_sem_signal(get_eth_recv_sem());
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_RIS | ETH_DMA_FLAG_NIS);
    }
    
    ENABLE_INTERRUPT(eth_irq_lock);
}

/**
 * @brief  In this function, the hardware should be initialized.
 * @param  netif                         The already initialized network interface structure for this ethernetif.
 * @retval int32_t:
 *           - LL_OK: Initialize success
 *           - LL_ERR: Initialize failed
 */
int32_t low_level_init(struct netif *netif)
{
    int32_t i32Ret = LL_ERR;
    stc_eth_init_t stcEthInit;
    uint16_t u16RegVal;

    /* Enable ETH clock */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_ETHMAC, ENABLE);
    /* Init Ethernet GPIO */
    Ethernet_GpioInit();
    /* Reset ETHERNET */
    (void)ETH_DeInit();
    /* Configure structure initialization */
    (void)ETH_CommStructInit(&EthHandle.stcCommInit);
    (void)ETH_StructInit(&stcEthInit);

#ifdef ETH_INTERFACE_RMII
    EthHandle.stcCommInit.u32Interface  = ETH_MAC_IF_RMII;
#else
    EthHandle.stcCommInit.u32Interface  = ETH_MAC_IF_MII;
#endif
    // stcEthInit.stcMacInit.u32ReceiveAll = ETH_MAC_RX_ALL_ENABLE;
    EthHandle.stcCommInit.u32ReceiveMode = ETH_RX_MD_INT;
    // EthHandle.stcCommInit.u32ChecksumMode = ETH_MAC_CHECKSUM_MD_SW;

    // install irq
    sys_sem_new(get_eth_recv_sem(), 0);
    hc32_install_irq_handler(&eth_irq_config, eth_irq_handler, ENABLE);

    /* Configure ethernet peripheral */
    if (LL_OK == ETH_Init(&EthHandle, &stcEthInit)) {
        u8EthInitStatus = 1U;
        i32Ret = LL_OK;
    }

#ifdef ETHERNET_LOOPBACK_TEST
    /* Enable PHY loopback */
    (void)ETH_PHY_LoopBackCmd(&EthHandle, ENABLE);
#endif

    /* Initialize Tx Descriptors list: Chain Mode */
    (void)ETH_DMA_TxDescListInit(&EthHandle, EthDmaTxDscrTab, &EthTxBuff[0][0], ETH_TX_BUF_NUM);
    /* Initialize Rx Descriptors list: Chain Mode  */
    (void)ETH_DMA_RxDescListInit(&EthHandle, EthDmaRxDscrTab, &EthRxBuff[0][0], ETH_RX_BUF_NUM);

    /* set MAC hardware address length */
    netif->hwaddr_len = 6U;
    /* set MAC hardware address */
    EthHandle.stcCommInit.u16AutoNego = ETH_AUTO_NEGO_DISABLE;

    netif->hwaddr[0] = (EthHandle.stcCommInit).au8MacAddr[0];
    netif->hwaddr[1] = (EthHandle.stcCommInit).au8MacAddr[1];
    netif->hwaddr[2] = (EthHandle.stcCommInit).au8MacAddr[2];
    netif->hwaddr[3] = (EthHandle.stcCommInit).au8MacAddr[3];
    netif->hwaddr[4] = (EthHandle.stcCommInit).au8MacAddr[4];
    netif->hwaddr[5] = (EthHandle.stcCommInit).au8MacAddr[5];
    /* maximum transfer unit */
    netif->mtu = 1500U;

    /* device capabilities */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* Enable MAC and DMA transmission and reception */
    (void)ETH_Start();

    /* Configure PHY LED mode */
    u16RegVal = PHY_PAGE_ADDR_7;
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSR, u16RegVal);
    (void)ETH_PHY_ReadReg(&EthHandle, PHY_P7_IWLFR, &u16RegVal);
    MODIFY_REG16(u16RegVal, PHY_LED_SELECT, PHY_LED_SELECT_10);
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_P7_IWLFR, u16RegVal);
    u16RegVal = PHY_PAGE_ADDR_0;
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSR, u16RegVal);

#ifdef ETH_INTERFACE_RMII
    /* Disable Power Saving Mode */
    (void)ETH_PHY_ReadReg(&EthHandle, PHY_PSMR, &u16RegVal);
    CLR_REG16_BIT(u16RegVal, PHY_EN_PWR_SAVE);
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSMR, u16RegVal);

    /* Configure PHY to generate an interrupt when Eth Link state changes */
    u16RegVal = PHY_PAGE_ADDR_7;
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSR, u16RegVal);
    /* Enable Interrupt on change of link status */
    (void)ETH_PHY_ReadReg(&EthHandle, PHY_P7_IWLFR, &u16RegVal);
    SET_REG16_BIT(u16RegVal, PHY_INT_LINK_CHANGE);
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_P7_IWLFR, u16RegVal);
    u16RegVal = PHY_PAGE_ADDR_0;
    (void)ETH_PHY_WriteReg(&EthHandle, PHY_PSR, u16RegVal);
#endif
    
    return i32Ret;
}

/**
 * @brief  This function should do the actual transmission of the packet.
 * @param  netif                        The network interface structure for this ethernetif.
 * @param  p                            The MAC packet to send.
 * @retval int32_t:
 *           - LL_OK: The packet could be sent
 *           - LL_ERR: The packet couldn't be sent
 */
err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t i32Ret;
    struct pbuf *q;
    uint8_t *txBuffer;
    __IO stc_eth_dma_desc_t *DmaTxDesc;
    uint32_t byteCnt;
    uint32_t frameLength = 0UL;
    uint32_t bufferOffset;
    uint32_t payloadOffset;

    DmaTxDesc = EthHandle.stcTxDesc;
    txBuffer = (uint8_t *)((EthHandle.stcTxDesc)->u32Buf1Addr);
    bufferOffset = 0UL;
    /* Copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next) {
        /* If this buffer isn't available, goto error */
        if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMA_TXDESC_OWN)) {
            i32Ret = (err_t)ERR_USE;
            goto error;
        }

        /* Get bytes in current buffer */
        byteCnt = q->len;
        payloadOffset = 0UL;
        /* Check if the length of data to copy is bigger than Tx buffer size */
        while ((byteCnt + bufferOffset) > ETH_TX_BUF_SIZE) {
            /* Copy data to Tx buffer*/
            (void)memcpy((uint8_t *) & (txBuffer[bufferOffset]), (uint8_t *) & (((uint8_t *)q->payload)[payloadOffset]), (ETH_TX_BUF_SIZE - bufferOffset));
            /* Point to next descriptor */
            DmaTxDesc = (stc_eth_dma_desc_t *)(DmaTxDesc->u32Buf2NextDescAddr);
            /* Check if the buffer is available */
            if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMA_TXDESC_OWN)) {
                i32Ret = (err_t)ERR_USE;
                goto error;
            }

            txBuffer = (uint8_t *)(DmaTxDesc->u32Buf1Addr);
            byteCnt = byteCnt - (ETH_TX_BUF_SIZE - bufferOffset);
            payloadOffset = payloadOffset + (ETH_TX_BUF_SIZE - bufferOffset);
            frameLength = frameLength + (ETH_TX_BUF_SIZE - bufferOffset);
            bufferOffset = 0UL;
        }
        /* Copy the remaining bytes */
        (void)memcpy((uint8_t *) & (txBuffer[bufferOffset]), (uint8_t *) & (((uint8_t *)q->payload)[payloadOffset]), byteCnt);
        bufferOffset = bufferOffset + byteCnt;
        frameLength = frameLength + byteCnt;
    }
    /* Prepare transmit descriptors to give to DMA */
    if(LL_OK != ETH_DMA_SetTransFrame(&EthHandle, frameLength)) {
        KPrintf("[%s] Error sending eth DMA frame\n", __func__);
    }
    i32Ret = (err_t)ERR_OK;

error:
    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if (RESET != ETH_DMA_GetStatus(ETH_DMA_FLAG_UNS)) {
        /* Clear DMA UNS flag */
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_UNS);
        /* Resume DMA transmission */
        WRITE_REG32(CM_ETH->DMA_TXPOLLR, 0UL);
    }

    return i32Ret;
}

/**
 * @brief  Should allocate a pbuf and transfer the bytes of the incoming packet from the interface into the pbuf.
 * @param  netif                        The network interface structure for this ethernetif.
 * @retval A pbuf filled with the received packet (including MAC header) or NULL on memory error.
 */
struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;
    uint8_t *rxBuffer;
    __IO stc_eth_dma_desc_t *DmaRxDesc;
    uint32_t byteCnt;
    uint32_t bufferOffset;
    uint32_t payloadOffset;
    uint32_t i;

    /* Get received frame */
    if (LL_OK != ETH_DMA_GetReceiveFrame(&EthHandle)) {
        return NULL;
    }

    /* Obtain the size of the packet */
    len = (EthHandle.stcRxFrame).u32Len;
    rxBuffer = (uint8_t *)(EthHandle.stcRxFrame).u32Buf;
    if (len > 0UL) {
        /* Allocate a pbuf chain of pbufs from the buffer */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        // p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    }
    if (p != NULL) {
        DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
        bufferOffset = 0UL;
        for (q = p; q != NULL; q = q->next) {
            byteCnt = q->len;
            payloadOffset = 0UL;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size */
            while ((byteCnt + bufferOffset) > ETH_RX_BUF_SIZE) {
                /* Copy data to pbuf */
                (void)memcpy((uint8_t *) & (((uint8_t *)q->payload)[payloadOffset]), (uint8_t *) & (rxBuffer[bufferOffset]), (ETH_RX_BUF_SIZE - bufferOffset));
                /* Point to next descriptor */
                DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buf2NextDescAddr);
                rxBuffer = (uint8_t *)(DmaRxDesc->u32Buf1Addr);
                byteCnt = byteCnt - (ETH_RX_BUF_SIZE - bufferOffset);
                payloadOffset = payloadOffset + (ETH_RX_BUF_SIZE - bufferOffset);
                bufferOffset = 0UL;
            }

            /* Copy remaining data in pbuf */
            (void)memcpy((uint8_t *) & (((uint8_t *)q->payload)[payloadOffset]), (uint8_t *) & (rxBuffer[bufferOffset]), byteCnt);
            bufferOffset = bufferOffset + byteCnt;
        }
    }
    /* Release descriptors to DMA */
    DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
    for (i = 0UL; i < (EthHandle.stcRxFrame).u32SegCount; i++) {
        DmaRxDesc->u32ControlStatus |= ETH_DMA_RXDESC_OWN;
        DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buf2NextDescAddr);
    }
    /* Clear Segment_Count */
    (EthHandle.stcRxFrame).u32SegCount = 0UL;

    /* When Rx Buffer unavailable flag is set, clear it and resume reception */
    if (RESET != ETH_DMA_GetStatus(ETH_DMA_FLAG_RUS)) {
        /* Clear DMA RUS flag */
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_RUS);
        /* Resume DMA reception */
        WRITE_REG32(CM_ETH->DMA_RXPOLLR, 0UL);
    }

    return p;
}

extern void LwipSetIPTest(int argc, char *argv[]);
int HwEthInit(void) {
//   lwip_config_tcp(0, lwip_ipaddr, lwip_netmask, lwip_gwaddr);
  LwipSetIPTest(1, NULL);
  return EOK;
}


