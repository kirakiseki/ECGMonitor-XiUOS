/**
********************************************************************************
\file   edrv-xiuos.c

\brief  Implementation of XiUOS Ethernet driver

This file contains the implementation of the XiUOS Ethernet driver.

\ingroup module_edrv
*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2017, BE.services GmbH
Copyright (c) 2017, B&R Industrial Automation GmbH
Copyright (c) 2020, AIIT XUOS Lab
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <common/oplkinc.h>
#include <common/ftracedebug.h>
#include <kernel/edrv.h>
#include <common/target.h>

#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#include <xsconfig.h>
#include <sys_arch.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define EDRV_MAX_FRAME_SIZE     0x0600
#define PROTO_PLK               0x88AB
#ifndef PACKET_QDISC_BYPASS
#define PACKET_QDISC_BYPASS     20
#endif
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
/**
\brief Structure describing an instance of the Edrv

This structure describes an instance of the Ethernet driver.
*/
typedef struct
{
    tEdrvInitParam      initParam;                       ///< Init parameters
    tEdrvTxBuffer*      pTransmittedTxBufferLastEntry;   ///< Pointer to the last entry of the transmitted TX buffer
    tEdrvTxBuffer*      pTransmittedTxBufferFirstEntry;  ///< Pointer to the first entry of the transmitted Tx buffer
    pthread_mutex_t     mutex;                           ///< Mutex for locking of critical sections
    BOOL                fStartCommunication;             ///< Flag to indicate, that communication is started. Set to false on exit
    struct netif*       pNetif;
} tEdrvInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tEdrvInstance edrvInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void     packetHandler(void* pParam_p,
                              const int frameSize_p,
                              void* pPktData_p);
static err_t    ethernetInput(struct pbuf* pPbuf_p, struct netif *pNetif_p);
static BOOL     getLinkStatus(const char* pIfName_p);

//------------------------------------------------------------------------------
// XiUOS implementaion of raw socket
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief  Ethernet driver initialization

This function initializes the Ethernet driver.

\param[in]      pEdrvInitParam_p    Edrv initialization parameters

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_init(const tEdrvInitParam* pEdrvInitParam_p)
{
    uint8_t enet_port = 0; ///< use enet port 0

    // Check parameter validity
    ASSERT(pEdrvInitParam_p != NULL);

    // Clear instance structure
    OPLK_MEMSET(&edrvInstance_l, 0, sizeof(edrvInstance_l));

    if (pEdrvInitParam_p->pDevName == NULL)
        return kErrorEdrvInit;

    // Save the init data
    edrvInstance_l.initParam = *pEdrvInitParam_p;

    edrvInstance_l.fStartCommunication = TRUE;

    lwip_config_net(enet_port, lwip_ipaddr, lwip_netmask, lwip_gwaddr);
    gnetif.input = ethernetInput;
    edrvInstance_l.pNetif = &gnetif;

    // If no MAC address was specified read MAC address of used
    // Ethernet interface
    if ((edrvInstance_l.initParam.aMacAddr[0] == 0) &&
        (edrvInstance_l.initParam.aMacAddr[1] == 0) &&
        (edrvInstance_l.initParam.aMacAddr[2] == 0) &&
        (edrvInstance_l.initParam.aMacAddr[3] == 0) &&
        (edrvInstance_l.initParam.aMacAddr[4] == 0) &&
        (edrvInstance_l.initParam.aMacAddr[5] == 0))
    {
        OPLK_MEMCPY(edrvInstance_l.initParam.aMacAddr, gnetif.hwaddr, 6);
    }
    if (pthread_mutex_init(&edrvInstance_l.mutex, NULL) != 0)
    {
        DEBUG_LVL_ERROR_TRACE("%s() couldn't init mutex\n", __func__);
        return kErrorEdrvInit;
    }

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Shut down Ethernet driver

This function shuts down the Ethernet driver.

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_exit(void)
{
    edrvInstance_l.fStartCommunication = FALSE;

    // Clear instance structure
    OPLK_MEMSET(&edrvInstance_l, 0, sizeof(edrvInstance_l));

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Get MAC address

This function returns the MAC address of the Ethernet controller

\return The function returns a pointer to the MAC address.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
const UINT8* edrv_getMacAddr(void)
{
    return edrvInstance_l.initParam.aMacAddr;
}

//------------------------------------------------------------------------------
/**
\brief  Send Tx buffer

This function sends the Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_sendTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    err_t         netifRet;
    struct pbuf*  pPbuf;

    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    FTRACE_MARKER("%s", __func__);

    if (pBuffer_p->txBufferNumber.pArg != NULL)
        return kErrorInvalidOperation;

    if (getLinkStatus(edrvInstance_l.initParam.pDevName) == FALSE)
    {
        /* If there is no link, we pretend that the packet is sent and immediately call
         * tx handler. Otherwise the stack would hang! */
        if (pBuffer_p->pfnTxHandler != NULL)
        {
            pBuffer_p->pfnTxHandler(pBuffer_p);
        }
    }
    else
    {
        pthread_mutex_lock(&edrvInstance_l.mutex);
        if (edrvInstance_l.pTransmittedTxBufferLastEntry == NULL)
        {
            edrvInstance_l.pTransmittedTxBufferLastEntry = pBuffer_p;
            edrvInstance_l.pTransmittedTxBufferFirstEntry = pBuffer_p;
        }
        else
        {
            edrvInstance_l.pTransmittedTxBufferLastEntry->txBufferNumber.pArg = pBuffer_p;
            edrvInstance_l.pTransmittedTxBufferLastEntry = pBuffer_p;
        }
        pthread_mutex_unlock(&edrvInstance_l.mutex);

        pPbuf = pbuf_alloc_reference(pBuffer_p->pBuffer, pBuffer_p->txFrameSize, PBUF_REF);
        ASSERT(pPbuf != NULL);
        
        netifRet = edrvInstance_l.pNetif->linkoutput(edrvInstance_l.pNetif, pPbuf);
        
        pbuf_free(pPbuf);
        
        if (netifRet < 0)
        {
            DEBUG_LVL_EDRV_TRACE("%s() linkoutput() returned %d\n", __func__, netifRet);
            return kErrorInvalidOperation;
        }
        else
        {
            packetHandler((u_char*)&edrvInstance_l, pBuffer_p->txFrameSize, pBuffer_p->pBuffer);
        }
    }

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Allocate Tx buffer

This function allocates a Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_allocTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    if (pBuffer_p->maxBufferSize > EDRV_MAX_FRAME_SIZE)
        return kErrorEdrvNoFreeBufEntry;

    // allocate buffer with malloc
    pBuffer_p->pBuffer = OPLK_MALLOC(pBuffer_p->maxBufferSize);
    if (pBuffer_p->pBuffer == NULL)
        return kErrorEdrvNoFreeBufEntry;

    pBuffer_p->txBufferNumber.pArg = NULL;

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Free Tx buffer

This function releases the Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_freeTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    void*   pBuffer;

    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    pBuffer = pBuffer_p->pBuffer;

    // mark buffer as free, before actually freeing it
    pBuffer_p->pBuffer = NULL;

    OPLK_FREE(pBuffer);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Change Rx filter setup

This function changes the Rx filter setup. The parameter entryChanged_p
selects the Rx filter entry that shall be changed and \p changeFlags_p determines
the property.
If \p entryChanged_p is equal or larger count_p all Rx filters shall be changed.

\note Rx filters are not supported by this driver!

\param[in,out]  pFilter_p           Base pointer of Rx filter array
\param[in]      count_p             Number of Rx filter array entries
\param[in]      entryChanged_p      Index of Rx filter entry that shall be changed
\param[in]      changeFlags_p       Bit mask that selects the changing Rx filter property

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_changeRxFilter(tEdrvFilter* pFilter_p,
                               UINT count_p,
                               UINT entryChanged_p,
                               UINT changeFlags_p)
{
    UNUSED_PARAMETER(pFilter_p);
    UNUSED_PARAMETER(count_p);
    UNUSED_PARAMETER(entryChanged_p);
    UNUSED_PARAMETER(changeFlags_p);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Clear multicast address entry

This function removes the multicast entry from the Ethernet controller.

\note The multicast filters are not supported by this driver.

\param[in]      pMacAddr_p          Multicast address

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_clearRxMulticastMacAddr(const UINT8* pMacAddr_p)
{
    UNUSED_PARAMETER(pMacAddr_p);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Set multicast address entry

This function sets a multicast entry into the Ethernet controller.

\note The multicast filters are not supported by this driver.

\param[in]      pMacAddr_p          Multicast address.

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_setRxMulticastMacAddr(const UINT8* pMacAddr_p)
{
    UNUSED_PARAMETER(pMacAddr_p);

    return kErrorOk;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief  Edrv packet handler

This function is the packet handler forwarding the frames to the dllk.

\param[in,out]  pParam_p            User specific pointer pointing to the instance structure
\param[in]      frameSize_p         Framesize information
\param[in]      pPktData_p          Packet buffer
*/
//------------------------------------------------------------------------------
static void packetHandler(void* pParam_p,
                          const int frameSize_p,
                          void* pPktData_p)
{
    tEdrvInstance*  pInstance = (tEdrvInstance*)pParam_p;
    tEdrvRxBuffer   rxBuffer;

    if (OPLK_MEMCMP((UINT8*)pPktData_p + 6, pInstance->initParam.aMacAddr, 6) != 0)
    {   // filter out self generated traffic
        rxBuffer.bufferInFrame = kEdrvBufferLastInFrame;
        rxBuffer.rxFrameSize = frameSize_p;
        rxBuffer.pBuffer = pPktData_p;

        FTRACE_MARKER("%s RX", __func__);
        pInstance->initParam.pfnRxHandler(&rxBuffer);
    }
    else
    {   // self generated traffic
        FTRACE_MARKER("%s TX-receive", __func__);

        if (pInstance->pTransmittedTxBufferFirstEntry != NULL)
        {
            tEdrvTxBuffer* pTxBuffer = pInstance->pTransmittedTxBufferFirstEntry;

            if (pTxBuffer->pBuffer != NULL)
            {
                if (OPLK_MEMCMP(pPktData_p, pTxBuffer->pBuffer, 6) == 0)
                {   // compare with packet buffer with destination MAC
                    pthread_mutex_lock(&pInstance->mutex);
                    pInstance->pTransmittedTxBufferFirstEntry = (tEdrvTxBuffer*)pInstance->pTransmittedTxBufferFirstEntry->txBufferNumber.pArg;
                    if (pInstance->pTransmittedTxBufferFirstEntry == NULL)
                    {
                        pInstance->pTransmittedTxBufferLastEntry = NULL;
                    }
                    pthread_mutex_unlock(&pInstance->mutex);

                    pTxBuffer->txBufferNumber.pArg = NULL;

                    if (pTxBuffer->pfnTxHandler != NULL)
                    {
                        pTxBuffer->pfnTxHandler(pTxBuffer);
                    }
                }
                else
                {
                    TRACE("%s: no matching TxB: DstMAC=%02X%02X%02X%02X%02X%02X\n",
                          __func__,
                          (UINT)((UINT8*)pPktData_p)[0],
                          (UINT)((UINT8*)pPktData_p)[1],
                          (UINT)((UINT8*)pPktData_p)[2],
                          (UINT)((UINT8*)pPktData_p)[3],
                          (UINT)((UINT8*)pPktData_p)[4],
                          (UINT)((UINT8*)pPktData_p)[5]);
                    TRACE("   current TxB %p: DstMAC=%02X%02X%02X%02X%02X%02X\n",
                          (void*)pTxBuffer,
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[0],
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[1],
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[2],
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[3],
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[4],
                          (UINT)((UINT8*)(pTxBuffer->pBuffer))[5]);
                }
            }
        }
        else
        {
            TRACE("%s: no TxB: DstMAC=%02X%02X%02X%02X%02X%02X\n", __func__,
                  ((UINT8*)pPktData_p)[0],
                  ((UINT8*)pPktData_p)[1],
                  ((UINT8*)pPktData_p)[2],
                  ((UINT8*)pPktData_p)[3],
                  ((UINT8*)pPktData_p)[4],
                  ((UINT8*)pPktData_p)[5]);
        }
    }
}

static err_t ethernetInput(struct pbuf* pPbuf_p, struct netif* pNetif_p)
{
    int           frameSize;
    struct pbuf*  pPbuf;

    UNUSED_PARAMETER(pNetif_p);

    if (edrvInstance_l.fStartCommunication == FALSE)
    {
        return ERR_ARG;
    }

    frameSize = pPbuf_p->tot_len;
    pPbuf = pbuf_coalesce(pPbuf_p, PBUF_RAW);
    ASSERT(pPbuf->len == frameSize);
    
    packetHandler(&edrvInstance_l, frameSize, pPbuf->payload);
    
    pbuf_free(pPbuf);

    return ERR_OK;
}

//------------------------------------------------------------------------------
/**
\brief  Get link status

This function returns the interface link status.

\param[in]      pIfName_p           Ethernet interface device name

\return The function returns the link status.
\retval TRUE    The link is up.
\retval FALSE   The link is down.
*/
//------------------------------------------------------------------------------
static BOOL getLinkStatus(const char* pIfName_p)
{
    UNUSED_PARAMETER(pIfName_p);
    return TRUE;
}
