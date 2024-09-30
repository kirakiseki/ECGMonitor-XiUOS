/*
* Copyright (c) 2020 AIIT XUOS Lab
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
* @file connect_usart.c
* @brief support ch32v307 vct6 uart function and register to bus framework
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-08-01
*/

#include <xizi.h>
#include "ch32v30x.h"
#include "xsconfig.h"
#include "connect_uart.h"
#include "ch32v30x_usart.h"

/*  uart driver */

static void SerialCfgParamCheck(struct SerialCfgParam* serial_cfg_default, struct SerialCfgParam* serial_cfg_new)
{
    struct SerialDataCfg *data_cfg_default = &serial_cfg_default->data_cfg;
    struct SerialDataCfg *data_cfg_new = &serial_cfg_new->data_cfg;

    if ((data_cfg_default->serial_baud_rate != data_cfg_new->serial_baud_rate) && (data_cfg_new->serial_baud_rate)) {
        data_cfg_default->serial_baud_rate = data_cfg_new->serial_baud_rate;
    }

    if ((data_cfg_default->serial_bit_order != data_cfg_new->serial_bit_order) && (data_cfg_new->serial_bit_order)) {
        data_cfg_default->serial_bit_order = data_cfg_new->serial_bit_order;
    }

    if ((data_cfg_default->serial_buffer_size != data_cfg_new->serial_buffer_size) && (data_cfg_new->serial_buffer_size)) {
        data_cfg_default->serial_buffer_size = data_cfg_new->serial_buffer_size;
    }

    if ((data_cfg_default->serial_data_bits != data_cfg_new->serial_data_bits) && (data_cfg_new->serial_data_bits)) {
        data_cfg_default->serial_data_bits = data_cfg_new->serial_data_bits;
    }

    if ((data_cfg_default->serial_invert_mode != data_cfg_new->serial_invert_mode) && (data_cfg_new->serial_invert_mode)) {
        data_cfg_default->serial_invert_mode = data_cfg_new->serial_invert_mode;
    }

    if ((data_cfg_default->serial_parity_mode != data_cfg_new->serial_parity_mode) && (data_cfg_new->serial_parity_mode)) {
        data_cfg_default->serial_parity_mode = data_cfg_new->serial_parity_mode;
    }

    if ((data_cfg_default->serial_stop_bits != data_cfg_new->serial_stop_bits) && (data_cfg_new->serial_stop_bits)) {
        data_cfg_default->serial_stop_bits = data_cfg_new->serial_stop_bits;
    }

    if ((data_cfg_default->serial_timeout != data_cfg_new->serial_timeout) && (data_cfg_new->serial_timeout)) {
        data_cfg_default->serial_timeout = data_cfg_new->serial_timeout;
    }
}

static void UartIsr(struct SerialDriver *serial_drv, struct SerialHardwareDevice *serial_dev)
{
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;
    
    if (RESET != USART_GetITStatus((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_IT_RXNE))
    {
        SerialSetIsr(serial_dev, SERIAL_EVENT_RX_IND);
        USART_ClearITPendingBit((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_IT_RXNE);
    }
}

static uint32 SerialInit(struct SerialDriver *serial_drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(serial_drv);
    struct SerialCfgParam* serial_cfg = (struct SerialCfgParam*)serial_drv->private_data;
    // struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;

    if (configure_info->private_data) {
        struct SerialCfgParam *serial_cfg_new = (struct SerialCfgParam *)configure_info->private_data;
        SerialCfgParamCheck(serial_cfg, serial_cfg_new);
    }

	struct SerialHardwareDevice *serial_dev = (struct SerialHardwareDevice *)serial_drv->driver.owner_bus->owner_haldev;
	struct SerialDevParam *dev_param = (struct SerialDevParam *)serial_dev->haldev.private_data;

	// config serial receive sem timeout
	dev_param->serial_timeout = serial_cfg->data_cfg.serial_timeout;

    // init usart type def
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = serial_cfg->data_cfg.serial_baud_rate;

    switch (serial_cfg->data_cfg.serial_data_bits)
    {
    case DATA_BITS_8:
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        break;

    case DATA_BITS_9:
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        break;
    default:
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        break;
    }
    
    switch (serial_cfg->data_cfg.serial_stop_bits)
    {
    case STOP_BITS_1:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;
    case STOP_BITS_2:
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
        break;
    default:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;
    }
    
    switch (serial_cfg->data_cfg.serial_parity_mode)
    {
    case PARITY_NONE:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    case PARITY_ODD:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;
    case PARITY_EVEN:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;
    default:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    }

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    // usart_hardware_flow_rts_config(serial_cfg->hw_cfg.serial_register_base, USART_RTS_DISABLE);
    // usart_hardware_flow_cts_config(serial_cfg->hw_cfg.serial_register_base, USART_CTS_DISABLE);

 
    return EOK;
}

static uint32 SerialConfigure(struct SerialDriver *serial_drv, int serial_operation_cmd)
{
    NULL_PARAM_CHECK(serial_drv);

    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;

    switch (serial_operation_cmd)
    {
    case OPER_CLR_INT:
        NVIC_DisableIRQ(serial_cfg->hw_cfg.serial_irq_interrupt);
        USART_ITConfig((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_IT_RXNE, DISABLE);
        break;
    case OPER_SET_INT:
        NVIC_EnableIRQ(serial_cfg->hw_cfg.serial_irq_interrupt);
        /* enable USART0 receive interrupt */
        USART_ITConfig((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_IT_RXNE, ENABLE);
        break;
    }
    
    return EOK;
}

static uint32 SerialDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    int serial_operation_cmd;
    struct SerialDriver *serial_drv = (struct SerialDriver *)drv;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            ret = SerialInit(serial_drv, configure_info);
            break;
        case OPE_CFG:
            serial_operation_cmd = *(int *)configure_info->private_data;
            ret = SerialConfigure(serial_drv, serial_operation_cmd);
            break;
        default:
            break;
    }

    return ret;
}

static int SerialPutChar(struct SerialHardwareDevice *serial_dev, char c)
{
    struct SerialCfgParam* serial_cfg = (struct SerialCfgParam*)serial_dev->private_data;

    while (USART_GetFlagStatus((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_FLAG_TXE) == RESET);
    USART_SendData((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, (uint8_t) c);
    return 0;
}

static int SerialGetChar(struct SerialHardwareDevice *serial_dev)
{
    int ch = -1;
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_dev->private_data;

    if (RESET != USART_GetFlagStatus((USART_TypeDef *)serial_cfg->hw_cfg.serial_register_base, USART_FLAG_RXNE))
    {
        ch = USART_ReceiveData((USART_TypeDef*)serial_cfg->hw_cfg.serial_register_base) & 0xff;
    }

    return ch;
}

static const struct SerialDataCfg data_cfg_init = 
{
    .serial_baud_rate = BAUD_RATE_115200,
    .serial_data_bits = DATA_BITS_8,
    .serial_stop_bits = STOP_BITS_1,
    .serial_parity_mode = PARITY_NONE,
    .serial_bit_order = BIT_ORDER_LSB,
    .serial_invert_mode = NRZ_NORMAL,
    .serial_buffer_size = SERIAL_RB_BUFSZ,
    .serial_timeout = WAITING_FOREVER,
};

/*manage the serial device operations*/
static const struct SerialDrvDone drv_done =
{
    .init = SerialInit,
    .configure = SerialConfigure,
};

/*manage the serial device hal operations*/
static struct SerialHwDevDone hwdev_done =
{
    .put_char = SerialPutChar,
    .get_char = SerialGetChar,
};

static int BoardSerialBusInit(struct SerialBus *serial_bus, struct SerialDriver *serial_driver, const char *bus_name, const char *drv_name)
{
    x_err_t ret = EOK;

    /*Init the serial bus */
    ret = SerialBusInit(serial_bus, bus_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the serial driver*/
    ret = SerialDriverInit(serial_driver, drv_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the serial driver to the serial bus*/
    ret = SerialDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDriverAttachToBus error %d\n", ret);
        return ERROR;
    } 

    return ret;
}

/*Attach the serial device to the serial bus*/
static int BoardSerialDevBend(struct SerialHardwareDevice *serial_device, void *serial_param, const char *bus_name, const char *dev_name)
{
    x_err_t ret = EOK;

    ret = SerialDeviceRegister(serial_device, serial_param, dev_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDeviceInit device %s error %d\n", dev_name, ret);
        return ERROR;
    }  

    ret = SerialDeviceAttachToBus(dev_name, bus_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDeviceAttachToBus device %s error %d\n", dev_name, ret);
        return ERROR;
    }  

    return  ret;
}

#ifdef BSP_USING_UART1
struct SerialDriver serial_driver_1;
struct SerialHardwareDevice serial_device_1;

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    GET_INT_SP();
    x_base level;
    level= DisableLocalInterrupt();
    isrManager.done->incCounter();
    EnableLocalInterrupt(level);
    UartIsr(&serial_driver_1, &serial_device_1);
    level = DisableLocalInterrupt();
    isrManager.done->decCounter();
    EnableLocalInterrupt(level);
    FREE_INT_SP();
}
#endif

int InitHwUart(void)
{
    x_err_t ret = EOK;

    static struct SerialBus serial_bus;
    memset(&serial_bus, 0, sizeof(struct SerialBus));
    
    memset(&serial_driver_1, 0, sizeof(struct SerialDriver));

    memset(&serial_device_1, 0, sizeof(struct SerialHardwareDevice));

    static struct SerialCfgParam serial_cfg;
    memset(&serial_cfg, 0, sizeof(struct SerialCfgParam));

    static struct SerialDevParam serial_dev_param;
    memset(&serial_dev_param, 0, sizeof(struct SerialDevParam));
    
    serial_driver_1.drv_done = &drv_done;
    serial_driver_1.configure = &SerialDrvConfigure;
    serial_device_1.hwdev_done = &hwdev_done;

    serial_cfg.data_cfg = data_cfg_init;

#ifdef BSP_USING_UART1
    serial_cfg.hw_cfg.serial_register_base = (uint32)USART1;
    serial_cfg.hw_cfg.serial_irq_interrupt = USART1_IRQn;
#endif
    
    serial_driver_1.private_data = (void*)&serial_cfg;

    serial_dev_param.serial_work_mode = SIGN_OPER_INT_RX;
    serial_device_1.haldev.private_data = (void *)&serial_dev_param;

    ret = BoardSerialBusInit(&serial_bus, &serial_driver_1, SERIAL_BUS_NAME_1, SERIAL_DRV_NAME_1);
    if (EOK != ret) {
        KPrintf("InitHwUart uarths error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardSerialDevBend(&serial_device_1, (void *)&serial_cfg, SERIAL_BUS_NAME_1, SERIAL_1_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("InitHwUart uarths error ret %u\n", ret);
        return ERROR;
    }

    GPIO_InitTypeDef gpio_init_struct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    gpio_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_init_struct);
    gpio_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init_struct);

    USART_InitTypeDef usart_init_struct;
    usart_init_struct.USART_BaudRate             = 115200;
    usart_init_struct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;
    usart_init_struct.USART_Mode                 = USART_Mode_Tx|USART_Mode_Rx;
    usart_init_struct.USART_WordLength           = USART_WordLength_8b;
    usart_init_struct.USART_StopBits             = USART_StopBits_1;
    usart_init_struct.USART_Parity               = USART_Parity_No;
    USART_Init((USART_TypeDef *)serial_cfg.hw_cfg.serial_register_base, &usart_init_struct);
    USART_Cmd((USART_TypeDef*)serial_cfg.hw_cfg.serial_register_base, ENABLE);
    
    return ret;
}