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
* @brief support vega-board uart function and register to bus framework
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-02-16
*/

#include <xizi.h>
#include <device.h>
#include <board.h>
#include "connect_uart.h"
#include "fsl_lpuart.h"


static void SerialCfgParamCheck(struct SerialCfgParam *serial_cfg_default, struct SerialCfgParam *serial_cfg_new)
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
    LPUART_Type *uart_base = (LPUART_Type *)serial_cfg->hw_cfg.serial_register_base;

    /* UART in mode Receiver */
    if (LPUART_GetStatusFlags(uart_base) & kLPUART_RxDataRegFullFlag)
    {
        SerialSetIsr(serial_dev, SERIAL_EVENT_RX_IND);
    }

    /* If RX overrun. */
    if (LPUART_STAT_OR_MASK & uart_base->STAT)
    {
        /* Clear overrun flag, otherwise the RX does not work. */
        uart_base->STAT = ((uart_base->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);
    }

}

static uint32 SerialInit(struct SerialDriver *serial_drv, struct BusConfigureInfo *configure_info)
{
    lpuart_config_t config;

    NULL_PARAM_CHECK(serial_drv);
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;

    if (configure_info->private_data) {
        struct SerialCfgParam *serial_cfg_new = (struct SerialCfgParam *)configure_info->private_data;
        SerialCfgParamCheck(serial_cfg, serial_cfg_new);
    }

	struct SerialHardwareDevice *serial_dev = (struct SerialHardwareDevice *)serial_drv->driver.owner_bus->owner_haldev;
	struct SerialDevParam *dev_param = (struct SerialDevParam *)serial_dev->haldev.private_data;

	// config serial receive sem timeout
	dev_param->serial_timeout = serial_cfg->data_cfg.serial_timeout;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = serial_cfg->data_cfg.serial_baud_rate;
    
    switch (serial_cfg->data_cfg.serial_data_bits)
    {
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT        
    case DATA_BITS_7:
        config.dataBitsCount = kLPUART_SevenDataBits;
        break;
#endif

    default:
        config.dataBitsCount = kLPUART_EightDataBits;
        break;
    }
    
    switch (serial_cfg->data_cfg.serial_stop_bits)
    {
    case STOP_BITS_2:
        config.stopBitCount = kLPUART_TwoStopBit;
        break;
    default:
        config.stopBitCount = kLPUART_OneStopBit;
        break;
    }
    
    switch (serial_cfg->data_cfg.serial_parity_mode)
    {
    case PARITY_ODD:
        config.parityMode = kLPUART_ParityOdd;
        break;
    case PARITY_EVEN:
        config.parityMode = kLPUART_ParityEven;
        break;
    default:
        config.parityMode = kLPUART_ParityDisabled;
        break;
    }

    config.enableTx = 1;
    config.enableRx = 1;

    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcFircAsync);

    uint32_t uartClkSrcFreq0 = CLOCK_GetIpFreq(kCLOCK_Lpuart0);
    LPUART_Init((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base, &config, uartClkSrcFreq0);
    LPUART_EnableInterrupts((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base, kLPUART_RxDataRegFullInterruptEnable);
    
    CLOCK_SetIpSrc(kCLOCK_Lpuart1, kCLOCK_IpSrcFircAsync);

    uint32_t uartClkSrcFreq1 = CLOCK_GetIpFreq(kCLOCK_Lpuart1);
    LPUART_Init((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base, &config, uartClkSrcFreq1);
    LPUART_EnableInterrupts((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base, kLPUART_RxDataRegFullInterruptEnable);

 
    return EOK;
}

static uint32 SerialConfigure(struct SerialDriver *serial_drv, int serial_operation_cmd)
{
    NULL_PARAM_CHECK(serial_drv);

    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;

    switch (serial_operation_cmd)
    {
    case OPER_CLR_INT:
        DisableIRQ(serial_cfg->hw_cfg.serial_irq_interrupt);
        break;
    case OPER_SET_INT:
        EnableIRQ(serial_cfg->hw_cfg.serial_irq_interrupt);
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
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_dev->private_data;

    LPUART_WriteByte((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base, (uint8_t) c);
    while (!(LPUART_GetStatusFlags((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base) & kLPUART_TxDataRegEmptyFlag));

    return 0;
}

static int SerialGetChar(struct SerialHardwareDevice *serial_dev)
{
    int ch = -1;
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_dev->private_data;

    if (LPUART_GetStatusFlags((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base) & kLPUART_RxDataRegFullFlag)
        ch = LPUART_ReadByte((LPUART_Type *)serial_cfg->hw_cfg.serial_register_base);

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
        return -ERROR;
    }

    /*Init the serial driver*/
    ret = SerialDriverInit(serial_driver, drv_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDriverInit error %d\n", ret);
        return -ERROR;
    }

    /*Attach the serial driver to the serial bus*/
    ret = SerialDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("InitHwUart SerialDriverAttachToBus error %d\n", ret);
        return -ERROR;
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
        return -ERROR;
    }  

    return  ret;
}
#ifdef BSP_USING_UART0
struct SerialDriver serial_driver_0;
struct SerialHardwareDevice serial_device_0;

void LPUART0_IRQHandler(int irq_num, void *arg)
{
    UartIsr(&serial_driver_0, &serial_device_0);
}
#endif

int InitHwUart(void)
{
    x_err_t ret = EOK;

#ifdef BSP_USING_UART0
    static struct SerialBus serial_bus;
    memset(&serial_bus, 0, sizeof(struct SerialBus));
    
    memset(&serial_driver_0, 0, sizeof(struct SerialDriver));

    memset(&serial_device_0, 0, sizeof(struct SerialHardwareDevice));

    static struct SerialCfgParam serial_cfg;
    memset(&serial_cfg, 0, sizeof(struct SerialCfgParam));

    static struct SerialDevParam serial_dev_param;
    memset(&serial_dev_param, 0, sizeof(struct SerialDevParam));
    
    serial_driver_0.drv_done = &drv_done;
    serial_driver_0.configure = &SerialDrvConfigure;
    serial_device_0.hwdev_done = &hwdev_done;

    serial_cfg.data_cfg = data_cfg_init;

    serial_cfg.hw_cfg.serial_register_base   = (uint32)LPUART0;
    serial_cfg.hw_cfg.serial_irq_interrupt = LPUART0_IRQn;

    serial_driver_0.private_data = (void *)&serial_cfg;

    serial_dev_param.serial_work_mode = SIGN_OPER_INT_RX;
    serial_device_0.haldev.private_data = (void *)&serial_dev_param;

    ret = BoardSerialBusInit(&serial_bus, &serial_driver_0, SERIAL_BUS_NAME_0, SERIAL_DRV_NAME_0);
    if (EOK != ret) {
        KPrintf("InitHwUart uarths error ret %u\n", ret);
        return -ERROR;
    }

    ret = BoardSerialDevBend(&serial_device_0, (void *)&serial_cfg, SERIAL_BUS_NAME_0, SERIAL_0_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("InitHwUart uarths error ret %u\n", ret);
        return -ERROR;
    }  
#endif

    return ret;
}
