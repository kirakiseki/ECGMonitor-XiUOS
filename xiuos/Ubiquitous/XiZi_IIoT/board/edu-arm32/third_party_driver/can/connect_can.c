/*
 * Copyright (c) Guangzhou Xingyi Electronic  Technology Co., Ltd
 *
 * Change Logs:
 * Date               Author       Notes
 * 2014-7-4      alientek   first version
 */

/**
* @file connect_can.c
* @brief support hc32f4a0 can function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2023-02-20
*/

/*************************************************
File name: connect_can.c
Description: support can configure and spi bus register function for hc32f4a0
Others: connect_can.c for references
*************************************************/

#include "connect_can.h"

#define CAN_X (CM_CAN2)

#define CAN_TX_PORT (GPIO_PORT_D)
#define CAN_TX_PIN  (GPIO_PIN_07)
#define CAN_RX_PORT (GPIO_PORT_D)
#define CAN_RX_PIN  (GPIO_PIN_06)
#define CAN_TX_PIN_FUNC (GPIO_FUNC_62)
#define CAN_RX_PIN_FUNC (GPIO_FUNC_63)

#define INTSEL_REG              ((uint32_t)(&CM_INTC->SEL0))
#define CANX_IRQ_SRC            INT_SRC_CAN2_HOST
#define CANX_IRQ_NUM               17
#define IRQ_NUM_OFFSET             16

#define CAN_AF1_ID (0x123UL)
#define CAN_AF1_ID_MSK (0xFFFUL)
#define CAN_AF1_MSK_TYPE CAN_ID_STD
#define CAN_AF2_ID (0x005UL)
#define CAN_AF2_ID_MSK (0x00FUL)
#define CAN_AF2_MSK_TYPE CAN_ID_STD
#define CAN_AF3_ID (0x23UL)
#define CAN_AF3_ID_MSK (0xFFUL)
#define CAN_AF3_MSK_TYPE CAN_ID_STD

#ifdef CAN_USING_INTERRUPT
void CanIrqHandler(int vector, void *param)
{
    stc_can_error_info_t err_info;
    uint32_t status = CAN_GetStatusValue(CAN_X);
    uint32_t error = CAN_GetErrorInfo(CAN_X,&err_info);
    
    KPrintf("Irq entered\n");    

    CAN_ClearStatus(CAN_X, status);
}

static void CanIrqConfig(void)
{
    // register IRQ src in IRQn
    __IO uint32_t *INTC_SELx = (__IO uint32_t *)(INTSEL_REG+ 4U * (uint32_t)(CANX_IRQ_NUM));
    WRITE_REG32(*INTC_SELx, CANX_IRQ_SRC);
    isrManager.done->registerIrq(CANX_IRQ_NUM+IRQ_NUM_OFFSET,CanIrqHandler,NULL);
    isrManager.done->enableIrq(CANX_IRQ_NUM);
}
#endif

static void CanInit(struct CanDriverConfigure *can_drv_config)
{
    stc_can_init_t stcInit;
    stc_can_filter_config_t astcAFCfg[] = { \
        {CAN_AF1_ID, CAN_AF1_ID_MSK, CAN_AF1_MSK_TYPE}, \
        {CAN_AF2_ID, CAN_AF2_ID_MSK, CAN_AF2_MSK_TYPE}, \
        {CAN_AF3_ID, CAN_AF3_ID_MSK, CAN_AF3_MSK_TYPE}, \
    };

    CLK_SetCANClockSrc(CLK_CAN2,CLK_CANCLK_SYSCLK_DIV4);

    /* Set the function of CAN pins. */
    GPIO_SetFunc(CAN_TX_PORT, CAN_TX_PIN, CAN_TX_PIN_FUNC);
    GPIO_SetFunc(CAN_RX_PORT, CAN_RX_PIN, CAN_RX_PIN_FUNC);

    /* Initializes CAN. */
    (void)CAN_StructInit(&stcInit);
    stcInit.pstcFilter = astcAFCfg;
    stcInit.u16FilterSelect  = (CAN_FILTER1 | CAN_FILTER2 | CAN_FILTER3);

    // Driver's config
    stcInit.stcBitCfg.u32SJW = can_drv_config->tsjw;    
    stcInit.stcBitCfg.u32Prescaler = can_drv_config->brp;
    stcInit.stcBitCfg.u32TimeSeg1 = can_drv_config->tbs1;
    stcInit.stcBitCfg.u32TimeSeg2 = can_drv_config->tbs2;
    stcInit.u8WorkMode = can_drv_config->mode;

#ifdef CAN_USING_FD
    stcInit.stcFDCfg.u8TDCSSP = 16U;
    stcInit.stcFDCfg.u8CANFDMode = CAN_FD_MODE_ISO_11898;
    stcInit.stcFDCfg.stcFBT.u32SEG1 = 16U;
    stcInit.stcFDCfg.stcFBT.u32SEG2 = 4U;
    stcInit.stcFDCfg.stcFBT.u32SJW  = 4U;
    stcInit.stcFDCfg.stcFBT.u32Prescaler = 1U;
    (void)CAN_FD_Init(APP_CAN_UNIT, &stcInit);
#else
    FCG_Fcg1PeriphClockCmd(PWC_FCG1_CAN2, ENABLE);
    (void)CAN_Init(CAN_X, &stcInit);
#endif
    CAN_ClearStatus(CAN_X, 0xFFFFFFFFU);

#ifdef CAN_USING_INTERRUPT
    /* Configures the interrupts if needed. */
    CAN_IntCmd(CAN_X, CAN_INT_RX, ENABLE);
    CanIrqConfig();
#endif    
}

static uint32 CanConfig(void *can_drv_config)
{
    x_err_t ret = EOK;
    return ret;        
}

static uint32 CanDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    x_err_t ret = EOK;
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);
    struct CanDriverConfigure *can_drv_config;
    switch (configure_info->configure_cmd)
    {
        case OPE_INT: // can basic init
            can_drv_config = (struct CanDriverConfigure *)configure_info->private_data;
            CanInit(can_drv_config);
            break;
        case OPE_CFG: 
            CanConfig(configure_info->private_data);
            break;
        default:
            break;
    }

    return ret;
}


static uint32 CanWriteData(void * dev , struct BusBlockWriteParam *write_param)
{
    x_err_t ret=EOK;
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(write_param);
    struct CanSendConfigure *p_can_config = (struct CanSendConfigure*)write_param->buffer;
    stc_can_tx_frame_t can_frame_obj;
    memset(&can_frame_obj,0,sizeof(stc_can_tx_frame_t));

    // configure CAN's flag bit
    can_frame_obj.IDE = p_can_config->ide;
    if(1==p_can_config->ide){
        can_frame_obj.u32ID = p_can_config->exdid;
    }else{
        can_frame_obj.u32ID = p_can_config->stdid;
    } 
    can_frame_obj.RTR = p_can_config->rtr;
    
    memcpy(can_frame_obj.au8Data,p_can_config->data,p_can_config->data_lenth);
    can_frame_obj.DLC = p_can_config->data_lenth;

    //put frame_buffer in message queue    
    if(can_frame_obj.DLC){
        ret = CAN_FillTxFrame(CAN_X,CAN_TX_BUF_STB,&can_frame_obj);
        if(EOK != ret){
            KPrintf("CAN fill tx frame failed(CODE:%d)!\n",ret);
            return ERROR;            
        }
        CAN_StartTx(CAN_X,CAN_TX_REQ_STB_ONE);
    }
    return ret;
}

static uint32 CanReadData(void *dev , struct BusBlockReadParam *databuf)
{
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(databuf);
    x_err_t ret=EOK;
    stc_can_rx_frame_t frame_received;
    struct CanSendConfigure *p_can_config = (struct CanSendConfigure*)databuf->buffer;
    memset(&frame_received,0,sizeof(stc_can_rx_frame_t));

    ret = CAN_GetRxFrame(CAN_X, &frame_received);
    if(EOK != ret){
        // KPrintf("CAN recv frame failed(CODE:%d)!\n",ret);
        p_can_config->data_lenth = 0;
        return ERROR;            
    }

    //put message in frame_buffer
    p_can_config->ide = frame_received.IDE;
    p_can_config->rtr = frame_received.RTR;
    if(p_can_config->ide==1){
        p_can_config->exdid = frame_received.u32ID ;
    }else{
        p_can_config->stdid = frame_received.u32ID;
        p_can_config->exdid = frame_received.u32ID ;
    }
    p_can_config->data_lenth = frame_received.DLC;
    for(int i=0;i<p_can_config->data_lenth;i++){
        p_can_config->data[i] = frame_received.au8Data[i];
    }
    
    return frame_received.DLC;
}

static struct CanDevDone can_dev_done = 
{
    .open  = NONE,
    .close  = NONE,
    .write  = CanWriteData,
    .read   = CanReadData
};

static int BoardCanBusInit(struct CanBus *can_bus, struct CanDriver *can_driver)
{
    x_err_t ret = EOK;

    /*Init the can bus */
    ret = CanBusInit(can_bus, CAN_BUS_NAME_2);
    if (EOK != ret) {
        KPrintf("Board_can_init canBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the can driver*/
    ret = CanDriverInit(can_driver, CAN_DRIVER_NAME_2);
    if (EOK != ret) {
        KPrintf("Board_can_init canDriverInit error %d\n", ret);
        return ERROR;
    }
    /*Attach the can driver to the can bus*/
    ret = CanDriverAttachToBus(CAN_DRIVER_NAME_2, CAN_BUS_NAME_2);
    if (EOK != ret) {
        KPrintf("Board_can_init CanDriverAttachToBus error %d\n", ret);
        return ERROR;
    } 

    return ret;
}

/* Attach the can device to the can bus*/
static int BoardCanDevBend(void)
{
    x_err_t ret = EOK;
    static struct CanHardwareDevice can_device0;
    memset(&can_device0, 0, sizeof(struct CanHardwareDevice));

    can_device0.dev_done = &can_dev_done;

    ret = CanDeviceRegister(&can_device0, NONE, CAN_2_DEVICE_NAME_1);
    if (EOK != ret) {
        KPrintf("board_can_init CanDeviceInit device %s error %d\n", CAN_2_DEVICE_NAME_1, ret);
        return ERROR;
    }  

    ret = CanDeviceAttachToBus(CAN_2_DEVICE_NAME_1, CAN_BUS_NAME_2);
    if (EOK != ret) {
        KPrintf("board_can_init CanDeviceAttachToBus device %s error %d\n", CAN_2_DEVICE_NAME_1, ret);
        return ERROR;
    }  

    return ret;
}

int HwCanInit(void)
{
    x_err_t ret = EOK;

    static struct CanBus can_bus;
    memset(&can_bus, 0, sizeof(struct CanBus));

    static struct CanDriver can_driver;
    memset(&can_driver, 0, sizeof(struct CanDriver));

    can_driver.configure = &(CanDrvConfigure);
    
    ret = BoardCanBusInit(&can_bus, &can_driver);
    if (EOK != ret) {
      KPrintf(" can_bus_init %s error ret %u\n", CAN_BUS_NAME_2, ret);
      return ERROR;
    }

    ret = BoardCanDevBend();
    if (EOK != ret) {
        KPrintf("board_can_init error ret %u\n", ret);
        return ERROR;
    }
    return EOK;
}
