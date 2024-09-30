/*
/**
 *******************************************************************************
 * @file  usart/usart_uart_int/source/main.c
 * @brief This example demonstrates UART data receive and transfer by interrupt.
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
* @file connect_usart.c
* @brief support edu-arm32-board usart function and register to bus framework
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-13
*/

/*************************************************
File name: connect_usart.c
Description: support edu-arm32-board usart configure and usart bus register function
Others: take projects\ev_hc32f4a0_lqfp176\examples\usart\usart_uart_int\source\main.c for references
History: 
1. Date: 2022-09-13
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board usart configure, write and read
2. support edu-arm32-board usart bus device and driver register
*************************************************/

#include <board.h>
#include <connect_usart.h>

#if defined(BSP_USING_UART3)
#define USART3_RX_PORT                  (GPIO_PORT_B)
#define USART3_RX_PIN                   (GPIO_PIN_11)

#define USART3_TX_PORT                  (GPIO_PORT_B)
#define USART3_TX_PIN                   (GPIO_PIN_10)
#endif

#if defined(BSP_USING_UART6)
#define USART6_RX_PORT                  (GPIO_PORT_H)
#define USART6_RX_PIN                   (GPIO_PIN_06)

#define USART6_TX_PORT                  (GPIO_PORT_E)
#define USART6_TX_PIN                   (GPIO_PIN_06)
#endif

static void UartIsr(struct SerialBus *serial, struct SerialDriver *serial_drv, struct SerialHardwareDevice *serial_dev);

static x_err_t UartGpioInit(CM_USART_TypeDef *USARTx)
{
    x_err_t result = EOK;

    switch ((uint32)USARTx)
    {
#ifdef BSP_USING_UART3
    case (uint32)CM_USART3:
        GPIO_SetFunc(USART3_RX_PORT, USART3_RX_PIN, GPIO_FUNC_33);
        GPIO_SetFunc(USART3_TX_PORT, USART3_TX_PIN, GPIO_FUNC_32);
        break;
#endif
#ifdef BSP_USING_UART6
    case (uint32)CM_USART6:
        GPIO_SetFunc(USART6_RX_PORT, USART6_RX_PIN, GPIO_FUNC_37);
        GPIO_SetFunc(USART6_TX_PORT, USART6_TX_PIN, GPIO_FUNC_36);
        break;
#endif
    default:
        result = -1;
        break;
    }

    return result;
}

static void UartRxErrIsr(struct SerialBus *serial, struct SerialDriver *serial_drv, struct SerialHardwareDevice *serial_dev)
{
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;
    struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;
    
    if (SET == USART_GetStatus(serial_hw_cfg->uart_device, (USART_FLAG_OVERRUN | USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR))) {
        USART_ReadData(serial_hw_cfg->uart_device);
    }
    USART_ClearStatus(serial_hw_cfg->uart_device, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

#ifdef BSP_USING_UART3
struct SerialBus serial_bus_3;
struct SerialDriver serial_driver_3;
struct SerialHardwareDevice serial_device_3;

void Uart3RxIrqHandler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    SerialSetIsr(&serial_device_3, SERIAL_EVENT_RX_IND);

    ENABLE_INTERRUPT(lock);
}

void Uart3RxErrIrqHandler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    UartRxErrIsr(&serial_bus_3, &serial_driver_3, &serial_device_3);

    ENABLE_INTERRUPT(lock);
}
#endif

#ifdef BSP_USING_UART6
struct SerialBus serial_bus_6;
struct SerialDriver serial_driver_6;
struct SerialHardwareDevice serial_device_6;

void Uart6RxIrqHandler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    SerialSetIsr(&serial_device_6, SERIAL_EVENT_RX_IND);

    ENABLE_INTERRUPT(lock);
}

void Uart6RxErrIrqHandler(void)
{
    x_base lock = 0;
    lock = DISABLE_INTERRUPT();

    UartRxErrIsr(&serial_bus_6, &serial_driver_6, &serial_device_6);

    ENABLE_INTERRUPT(lock);
}
#endif

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

static uint32 SerialInit(struct SerialDriver *serial_drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(serial_drv);

    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;
    struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;

    if (configure_info->private_data) {
        struct SerialCfgParam *serial_cfg_new = (struct SerialCfgParam *)configure_info->private_data;
        SerialCfgParamCheck(serial_cfg, serial_cfg_new);
    }

	struct SerialHardwareDevice *serial_dev = (struct SerialHardwareDevice *)serial_drv->driver.owner_bus->owner_haldev;
	struct SerialDevParam *dev_param = (struct SerialDevParam *)serial_dev->haldev.private_data;

	// config serial receive sem timeout
	dev_param->serial_timeout = serial_cfg->data_cfg.serial_timeout;

    stc_usart_uart_init_t uart_init;
    USART_UART_StructInit(&uart_init);
    uart_init.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    uart_init.u32Baudrate = serial_cfg->data_cfg.serial_baud_rate;
    uart_init.u32ClockSrc = USART_CLK_SRC_INTERNCLK;

    if ((CM_USART1 == serial_hw_cfg->uart_device) || (CM_USART2 == serial_hw_cfg->uart_device) || \
            (CM_USART6 == serial_hw_cfg->uart_device) || (CM_USART7 == serial_hw_cfg->uart_device)) {
        uart_init.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    }

    switch (serial_cfg->data_cfg.serial_data_bits)
    {
    case DATA_BITS_8:
        uart_init.u32DataWidth = USART_DATA_WIDTH_8BIT;
        break;
    case DATA_BITS_9:
        uart_init.u32DataWidth = USART_DATA_WIDTH_9BIT;
        break;
    default:
        uart_init.u32DataWidth = USART_DATA_WIDTH_8BIT;
        break;
    }

    switch (serial_cfg->data_cfg.serial_stop_bits)
    {
    case STOP_BITS_1:
        uart_init.u32StopBit = USART_STOPBIT_1BIT;
        break;
    case STOP_BITS_2:
        uart_init.u32StopBit = USART_STOPBIT_2BIT;
        break;
    default:
        uart_init.u32StopBit = USART_STOPBIT_1BIT;
        break;
    }

    switch (serial_cfg->data_cfg.serial_parity_mode)
    {
    case PARITY_NONE:
        uart_init.u32Parity = USART_PARITY_NONE;
        break;
    case PARITY_EVEN:
        uart_init.u32Parity = USART_PARITY_EVEN;
        break;
    case PARITY_ODD:
        uart_init.u32Parity = USART_PARITY_ODD;
        break;
    default:
        uart_init.u32Parity = USART_PARITY_NONE;
        break;
    }

    if (BIT_ORDER_LSB == serial_cfg->data_cfg.serial_bit_order) {
        uart_init.u32FirstBit = USART_FIRST_BIT_LSB;
    } else {
        uart_init.u32FirstBit = USART_FIRST_BIT_MSB;
    }

    /* Enable USART clock */
    FCG_USART_CLK(serial_hw_cfg->usart_clock, ENABLE);
    if (EOK != UartGpioInit(serial_hw_cfg->uart_device)) {
        return ERROR;
    }

    /* Configure UART */
    uint32_t u32Div;
    float32_t f32Error;
    int32_t i32Ret = LL_ERR;
    USART_DeInit(serial_hw_cfg->uart_device);
    USART_UART_Init(serial_hw_cfg->uart_device, &uart_init, NULL);

    for (u32Div = 0UL; u32Div <= USART_CLK_DIV64; u32Div++) {
        USART_SetClockDiv(serial_hw_cfg->uart_device, u32Div);
        if ((LL_OK == USART_SetBaudrate(serial_hw_cfg->uart_device, uart_init.u32Baudrate, &f32Error)) &&
                ((-UART_BAUDRATE_ERR_MAX <= f32Error) && (f32Error <= UART_BAUDRATE_ERR_MAX))) {
            i32Ret = LL_OK;
            break;
        }
    }
    if (i32Ret != LL_OK) {
        return ERROR;
    }

    /* Enable error interrupt */
    NVIC_EnableIRQ(serial_hw_cfg->rx_err_irq.irq_config.irq_num);
    USART_FuncCmd(serial_hw_cfg->uart_device, USART_TX | USART_RX | USART_INT_RX, ENABLE);

    return EOK;
}

static uint32 SerialConfigure(struct SerialDriver *serial_drv, int serial_operation_cmd)
{
    NULL_PARAM_CHECK(serial_drv);

    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_drv->private_data;
    struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;

    switch (serial_operation_cmd)
    {
        case OPER_CLR_INT:
            NVIC_DisableIRQ(serial_hw_cfg->rx_irq.irq_config.irq_num);
            USART_FuncCmd(serial_hw_cfg->uart_device, USART_INT_RX, DISABLE);
            INTC_IrqSignOut(serial_hw_cfg->rx_irq.irq_config.irq_num);
            break;

        case OPER_SET_INT:
            hc32_install_irq_handler(&serial_hw_cfg->rx_irq.irq_config, serial_hw_cfg->rx_irq.irq_callback, 1);
            USART_FuncCmd(serial_hw_cfg->uart_device, USART_INT_RX, ENABLE);
            break;
    }

    return EOK;
}

static int SerialPutChar(struct SerialHardwareDevice *serial_dev, char c)
{
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_dev->private_data;
    struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;

    /* Polling mode. */
    while (USART_GetStatus(serial_hw_cfg->uart_device, USART_FLAG_TX_EMPTY) != SET);
    USART_WriteData(serial_hw_cfg->uart_device, c);

    return 1;
}

static int SerialGetChar(struct SerialHardwareDevice *serial_dev)
{
    struct SerialCfgParam *serial_cfg = (struct SerialCfgParam *)serial_dev->private_data;
    struct UsartHwCfg *serial_hw_cfg = (struct UsartHwCfg *)serial_cfg->hw_cfg.private_data;

    int c = -1;
    if (SET == USART_GetStatus(serial_hw_cfg->uart_device, USART_FLAG_RX_FULL)) {
        c = (uint8)USART_ReadData(serial_hw_cfg->uart_device);
    }

    return c;
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
        KPrintf("HwUartInit SerialBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the serial driver*/
    ret = SerialDriverInit(serial_driver, drv_name);
    if (EOK != ret) {
        KPrintf("HwUartInit SerialDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the serial driver to the serial bus*/
    ret = SerialDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("HwUartInit SerialDriverAttachToBus error %d\n", ret);
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
        KPrintf("HwUartInit SerialDeviceInit device %s error %d\n", dev_name, ret);
        return ERROR;
    }  

    ret = SerialDeviceAttachToBus(dev_name, bus_name);
    if (EOK != ret) {
        KPrintf("HwUartInit SerialDeviceAttachToBus device %s error %d\n", dev_name, ret);
        return ERROR;
    }  

    return  ret;
}

int HwUsartInit(void)
{
    x_err_t ret = EOK;

#ifdef BSP_USING_UART3
    static struct SerialCfgParam serial_cfg_3;
    memset(&serial_cfg_3, 0, sizeof(struct SerialCfgParam));

    static struct SerialDevParam serial_dev_param_3;
    memset(&serial_dev_param_3, 0, sizeof(struct SerialDevParam));
    
    static struct UsartHwCfg serial_hw_cfg_3;
    memset(&serial_hw_cfg_3, 0, sizeof(struct UsartHwCfg));

    serial_driver_3.drv_done = &drv_done;
    serial_driver_3.configure = SerialDrvConfigure;
    serial_device_3.hwdev_done = &hwdev_done;

    serial_cfg_3.data_cfg = data_cfg_init;

    //default irq configure
    serial_hw_cfg_3.uart_device = CM_USART3;
    serial_hw_cfg_3.usart_clock = FCG3_PERIPH_USART3;
    serial_hw_cfg_3.rx_err_irq.irq_config.irq_num = BSP_UART3_RXERR_IRQ_NUM;
    serial_hw_cfg_3.rx_err_irq.irq_config.irq_prio = BSP_UART3_RXERR_IRQ_PRIO;
    serial_hw_cfg_3.rx_err_irq.irq_config.int_src = INT_SRC_USART3_EI;

    serial_hw_cfg_3.rx_irq.irq_config.irq_num = BSP_UART3_RX_IRQ_NUM;
    serial_hw_cfg_3.rx_irq.irq_config.irq_prio = BSP_UART3_RX_IRQ_PRIO;
    serial_hw_cfg_3.rx_irq.irq_config.int_src = INT_SRC_USART3_RI;

    serial_hw_cfg_3.rx_err_irq.irq_callback = Uart3RxErrIrqHandler;
    serial_hw_cfg_3.rx_irq.irq_callback = Uart3RxIrqHandler;

    hc32_install_irq_handler(&serial_hw_cfg_3.rx_err_irq.irq_config, serial_hw_cfg_3.rx_err_irq.irq_callback, 0);

    serial_cfg_3.hw_cfg.private_data = (void *)&serial_hw_cfg_3;
    serial_driver_3.private_data = (void *)&serial_cfg_3;

    serial_dev_param_3.serial_work_mode = SIGN_OPER_INT_RX;
    serial_device_3.haldev.private_data = (void *)&serial_dev_param_3;

    ret = BoardSerialBusInit(&serial_bus_3, &serial_driver_3, SERIAL_BUS_NAME_3, SERIAL_DRV_NAME_3);
    if (EOK != ret) {
        KPrintf("HwUartInit uart3 error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardSerialDevBend(&serial_device_3, (void *)&serial_cfg_3, SERIAL_BUS_NAME_3, SERIAL_3_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("HwUartInit uart3 error ret %u\n", ret);
        return ERROR;
    }  
#endif

#ifdef BSP_USING_UART6
    static struct SerialCfgParam serial_cfg_6;
    memset(&serial_cfg_6, 0, sizeof(struct SerialCfgParam));

    static struct SerialDevParam serial_dev_param_6;
    memset(&serial_dev_param_6, 0, sizeof(struct SerialDevParam));
    
    static struct UsartHwCfg serial_hw_cfg_6;
    memset(&serial_hw_cfg_6, 0, sizeof(struct UsartHwCfg));

    serial_driver_6.drv_done = &drv_done;
    serial_driver_6.configure = SerialDrvConfigure;
    serial_device_6.hwdev_done = &hwdev_done;

    serial_cfg_6.data_cfg = data_cfg_init;

    //default irq configure
    serial_hw_cfg_6.uart_device = CM_USART6;
    serial_hw_cfg_6.usart_clock = FCG3_PERIPH_USART6;
    serial_hw_cfg_6.rx_err_irq.irq_config.irq_num = BSP_UART6_RXERR_IRQ_NUM;
    serial_hw_cfg_6.rx_err_irq.irq_config.irq_prio = BSP_UART6_RXERR_IRQ_PRIO;
    serial_hw_cfg_6.rx_err_irq.irq_config.int_src = INT_SRC_USART6_EI;

    serial_hw_cfg_6.rx_irq.irq_config.irq_num = BSP_UART6_RX_IRQ_NUM;
    serial_hw_cfg_6.rx_irq.irq_config.irq_prio = BSP_UART6_RX_IRQ_PRIO;
    serial_hw_cfg_6.rx_irq.irq_config.int_src = INT_SRC_USART6_RI;

    serial_hw_cfg_6.rx_err_irq.irq_callback = Uart6RxErrIrqHandler;
    serial_hw_cfg_6.rx_irq.irq_callback = Uart6RxIrqHandler;

    hc32_install_irq_handler(&serial_hw_cfg_6.rx_err_irq.irq_config, serial_hw_cfg_6.rx_err_irq.irq_callback, 0);

    serial_cfg_6.hw_cfg.private_data = (void *)&serial_hw_cfg_6;
    serial_driver_6.private_data = (void *)&serial_cfg_6;

    serial_dev_param_6.serial_work_mode = SIGN_OPER_INT_RX;
    serial_device_6.haldev.private_data = (void *)&serial_dev_param_6;

    ret = BoardSerialBusInit(&serial_bus_6, &serial_driver_6, SERIAL_BUS_NAME_6, SERIAL_DRV_NAME_6);
    if (EOK != ret) {
        KPrintf("HwUartInit uart6 error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardSerialDevBend(&serial_device_6, (void *)&serial_cfg_6, SERIAL_BUS_NAME_6, SERIAL_6_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("HwUartInit uart6 error ret %u\n", ret);
        return ERROR;
    }  
#endif

    return ret;
}
