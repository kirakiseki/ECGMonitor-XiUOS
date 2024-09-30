/*
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
* @file connect_i2c.c
* @brief support edu-arm32-board i2c function and register to bus framework
* @version 3.0 
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

/*************************************************
File name: connect_i2c.c
Description: support edu-arm32-board i2c configure and i2c bus register function
Others: take projects/ev_hc32f4a0_lqfp176/examples/i2c/i2c_master_polling/source/main.c for references
History: 
1. Date: 2022-12-05
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board i2c configure, write and read
2. support edu-arm32-board i2c bus device and driver register
*************************************************/

#include <connect_i2c.h>

#define I2C_UNIT                        (CM_I2C1)
#define I2C_FCG_USE                     (FCG1_PERIPH_I2C1)

#define I2C_TIMEOUT                     (0x40000UL)
#define I2C_BAUDRATE                    (400000UL)

/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT                    (GPIO_PORT_D)
#define I2C_SCL_PIN                     (GPIO_PIN_03)
#define I2C_SDA_PORT                    (GPIO_PORT_F)
#define I2C_SDA_PIN                     (GPIO_PIN_10)
#define I2C_GPIO_SCL_FUNC               (GPIO_FUNC_49)
#define I2C_GPIO_SDA_FUNC               (GPIO_FUNC_48)

static x_err_t I2cGpioInit(void)
{
    GPIO_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, I2C_GPIO_SDA_FUNC);
    GPIO_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, I2C_GPIO_SCL_FUNC);

    return EOK;
}

static uint32 I2cInit(struct I2cDriver *i2c_drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(i2c_drv);

    struct I2cHardwareDevice *i2c_dev = (struct I2cHardwareDevice *)i2c_drv->driver.owner_bus->owner_haldev; 

    stc_i2c_init_t i2c_init;
    (void)I2C_StructInit(&i2c_init);
    i2c_init.u32Baudrate = I2C_BAUDRATE;
    i2c_init.u32SclTime = 3UL;
    i2c_init.u32ClockDiv = I2C_CLK_DIV4;

    if (configure_info->private_data) {
        i2c_dev->i2c_dev_addr = *((uint16 *)configure_info->private_data);
    } else {
        KPrintf("I2cInit need set i2c dev addr\n");
        return ERROR;
    }

    /* Configure I2C */
    float32_t f32Error;
    int32_t i32Ret = LL_ERR;
    I2C_DeInit(I2C_UNIT);
    i32Ret = I2C_Init(I2C_UNIT, &i2c_init, &f32Error);

#ifdef I2C_DEVICE_SLAVE
    if (LL_OK == i32Ret) {
        /* Set slave address */
        I2C_SlaveAddrConfig(I2C_UNIT, I2C_ADDR0, I2C_ADDR_7BIT, i2c_dev->i2c_dev_addr);
    }   
#endif

    if(i32Ret != LL_OK) {
        return ERROR;
    }

    I2C_BusWaitCmd(I2C_UNIT, ENABLE);

    return EOK;
}

static uint32 I2cDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    struct I2cDriver *i2c_drv = (struct I2cDriver *)drv;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            ret = I2cInit(i2c_drv, configure_info);
            break;
        default:
            break;
    } 

    return ret;
}

static uint32 I2cMasterWriteData(struct I2cHardwareDevice *i2c_dev, struct I2cDataStandard *msg)
{
    uint32 i32Ret;

    I2C_Cmd(I2C_UNIT, ENABLE);
    DDL_DelayMS(20UL);
    I2C_SWResetCmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, DISABLE);
    DDL_DelayMS(20UL);
    i32Ret = I2C_Start(I2C_UNIT, I2C_TIMEOUT);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2C_UNIT, i2c_dev->i2c_dev_addr, I2C_DIR_TX, I2C_TIMEOUT);

        if (i32Ret == LL_OK) {
            i32Ret = I2C_TransData(I2C_UNIT, msg->buf, msg->len, I2C_TIMEOUT);
            KPrintf("Master Send Success!\n");
        }
    }

    (void)I2C_Stop(I2C_UNIT, I2C_TIMEOUT);
    I2C_Cmd(I2C_UNIT, DISABLE);

    return i32Ret;
}

static uint32 I2cMasterReadData(struct I2cHardwareDevice *i2c_dev, struct I2cDataStandard *msg)
{
    uint32 i32Ret;

    I2C_Cmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, DISABLE);
    i32Ret = I2C_Start(I2C_UNIT, I2C_TIMEOUT);
    if (LL_OK == i32Ret) {
        if (msg->len == 1U) {
            I2C_AckConfig(I2C_UNIT, I2C_NACK);
        }

        i32Ret = I2C_TransAddr(I2C_UNIT, i2c_dev->i2c_dev_addr, I2C_DIR_RX, I2C_TIMEOUT);

        if (LL_OK == i32Ret) {
            i32Ret = I2C_MasterReceiveDataAndStop(I2C_UNIT, msg->buf, msg->len, I2C_TIMEOUT);
            KPrintf("Master Receive Success!\n");
        }

        I2C_AckConfig(I2C_UNIT, I2C_ACK);
    }

    if (LL_OK != i32Ret) {
        (void)I2C_Stop(I2C_UNIT, I2C_TIMEOUT);
    }
    I2C_Cmd(I2C_UNIT, DISABLE);
    return i32Ret;
}

static uint32 I2cSlaveWriteData(struct I2cHardwareDevice *i2c_dev, struct I2cDataStandard *msg) {
    uint32 i32Ret;

    I2C_Cmd(I2C_UNIT, ENABLE);

    /* Clear status */
    I2C_ClearStatus(I2C_UNIT, I2C_CLR_STOPFCLR | I2C_CLR_NACKFCLR);

    /* Wait slave address matched */
    while (RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_MATCH_ADDR0)) {
        ;
    }

    I2C_ClearStatus(I2C_UNIT, I2C_CLR_SLADDR0FCLR);

    if (RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TRA)) {
        i32Ret = LL_ERR;
    } else {
        i32Ret = I2C_TransData(I2C_UNIT, msg->buf, msg->len, I2C_TIMEOUT);
		KPrintf("Slave send success!\r\n");

        if ((LL_OK == i32Ret) || (LL_ERR_TIMEOUT == i32Ret)) {
            /* Release SCL pin */
            (void)I2C_ReadData(I2C_UNIT);

            /* Wait stop condition */
            i32Ret = I2C_WaitStatus(I2C_UNIT, I2C_FLAG_STOP, SET, I2C_TIMEOUT);
        }
    }

    I2C_Cmd(I2C_UNIT, DISABLE);
    return i32Ret;
}

static uint32 I2cSlaveReadData(struct I2cHardwareDevice *i2c_dev, struct I2cDataStandard *msg) {
    uint32 i32Ret;

    I2C_Cmd(I2C_UNIT, ENABLE);

    /* Clear status */
    I2C_ClearStatus(I2C_UNIT, I2C_CLR_STOPFCLR | I2C_CLR_NACKFCLR);

    /* Wait slave address matched */
    while (RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_MATCH_ADDR0)) {
        ;
    }

    I2C_ClearStatus(I2C_UNIT, I2C_CLR_SLADDR0FCLR);

    if (RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TRA)) {
        /* Slave receive data*/
        i32Ret = I2C_ReceiveData(I2C_UNIT, msg->buf, msg->len, I2C_TIMEOUT);
		KPrintf("Slave receive success!\r\n");

        if ((LL_OK == i32Ret) || (LL_ERR_TIMEOUT == i32Ret)) {
            /* Wait stop condition */
            i32Ret = I2C_WaitStatus(I2C_UNIT, I2C_FLAG_STOP, SET, I2C_TIMEOUT);
        }
    } else {
        i32Ret = LL_ERR;
    }

    I2C_Cmd(I2C_UNIT, DISABLE);
    return i32Ret;
}


/* manage the i2c device operations*/
static const struct I2cDevDone i2c_dev_done =
{
    .dev_open = NONE,
    .dev_close = NONE,
#ifdef I2C_DEVICE_SLAVE
    .dev_write = I2cSlaveWriteData,
    .dev_read = I2cSlaveReadData,
#else
    .dev_write = I2cMasterWriteData,
    .dev_read = I2cMasterReadData,
#endif
};

/* Init i2c bus */
static int BoardI2cBusInit(struct I2cBus *i2c_bus, struct I2cDriver *i2c_driver)
{
    x_err_t ret = EOK;

    /* Init the i2c bus */
    ret = I2cBusInit(i2c_bus, I2C_BUS_NAME_1);
    if (EOK != ret) {
        KPrintf("board_i2c_init I2cBusInit error %d\n", ret);
        return ERROR;
    }

    /* Init the i2c driver*/
    ret = I2cDriverInit(i2c_driver, I2C_DRV_NAME_1);
    if (EOK != ret) {
        KPrintf("board_i2c_init I2cDriverInit error %d\n", ret);
        return ERROR;
    }

    /* Attach the i2c driver to the i2c bus*/
    ret = I2cDriverAttachToBus(I2C_DRV_NAME_1, I2C_BUS_NAME_1);
    if (EOK != ret) {
        KPrintf("board_i2c_init I2cDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

/* Attach the i2c device to the i2c bus*/
static int BoardI2cDevBend(void)
{
    x_err_t ret = EOK;
    static struct I2cHardwareDevice i2c_device0;
    memset(&i2c_device0, 0, sizeof(struct I2cHardwareDevice));

    i2c_device0.i2c_dev_done = &i2c_dev_done;

    ret = I2cDeviceRegister(&i2c_device0, NONE, I2C_1_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("board_i2c_init I2cDeviceInit device %s error %d\n", I2C_1_DEVICE_NAME_0, ret);
        return ERROR;
    }  

    ret = I2cDeviceAttachToBus(I2C_1_DEVICE_NAME_0, I2C_BUS_NAME_1);
    if (EOK != ret) {
        KPrintf("board_i2c_init I2cDeviceAttachToBus device %s error %d\n", I2C_1_DEVICE_NAME_0, ret);
        return ERROR;
    }  

    return ret;
}

/* EDU-ARM32 BOARD I2C INIT*/
int HwI2cInit(void)
{
    x_err_t ret = EOK;
    static struct I2cBus i2c_bus;
    memset(&i2c_bus, 0, sizeof(struct I2cBus));

    static struct I2cDriver i2c_driver;
    memset(&i2c_driver, 0, sizeof(struct I2cDriver));

    I2cGpioInit();

    /* Enable I2C Peripheral*/
    FCG_Fcg1PeriphClockCmd(I2C_FCG_USE, ENABLE);

    i2c_driver.configure = I2cDrvConfigure;

    ret = BoardI2cBusInit(&i2c_bus, &i2c_driver);
    if (ret != EOK) {
        KPrintf("board_i2c_init error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardI2cDevBend();
    if (EOK != ret) {
        KPrintf("board_i2c_init error ret %u\n", ret);
        return ERROR;
    }

    return ret;
}

//#define I2C_TEST
#ifdef I2C_TEST

#define USER_KEY_PORT                  (GPIO_PORT_I)
#define USER_KEY_PIN                   (GPIO_PIN_07)

#define DEVICE_ADDR                    (0x06U)

static struct Bus *bus1;
static struct I2cDriver *i2c_drv1;

void I2cInitTest(void)
{
    x_err_t ret = EOK;

    stc_gpio_init_t stcGpioInit;

    /* KEY initialize */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(USER_KEY_PORT, USER_KEY_PIN, &stcGpioInit);

    bus1 = BusFind(I2C_BUS_NAME_1);
    bus1->owner_driver = BusFindDriver(bus1, I2C_DRV_NAME_1);
    bus1->owner_haldev = BusFindDevice(bus1, I2C_1_DEVICE_NAME_0);

    struct BusConfigureInfo configure_info;
    configure_info.configure_cmd = OPE_INT;
    configure_info.private_data = (void *)DEVICE_ADDR;

    ret = I2cDrvConfigure(bus1->owner_driver, &configure_info);
    if (ret != EOK) {
        KPrintf("initialize %s failed!\n", I2C_UNIT);
        return;
    }

    i2c_drv1 = (struct I2cDriver *)bus1->owner_driver;

    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
I2cInitTest, I2cInitTest,  i2c init);

void I2cMasterTest(void)
{
    x_err_t ret = EOK;

    struct I2cHardwareDevice *i2c_dev1 = (struct I2cHardwareDevice *)i2c_drv1->driver.owner_bus->owner_haldev;

    struct I2cDataStandard msg;
    uint8 u8TxBuf[256U] = {1, 2, 3, 4, 5};
    uint8 u8RxBuf[256U];

    (void)memset(u8RxBuf, 0, 256U);

    msg.len = 5;
    msg.buf = u8TxBuf;

    while (SET == GPIO_ReadInputPins(USER_KEY_PORT, USER_KEY_PIN)) {
        ;
    }

    KPrintf("I2C send data\n");

    ret = I2cMasterWriteData(i2c_dev1, &msg);
    if (EOK != ret) {
        KPrintf("I2C send failed! ret %d\n", ret);
        return;
    }
    
    KPrintf("Master send data: ");
    
    for (uint16 i = 0; i < msg.len; i++) {
        KPrintf("%d ", (msg.buf)[i]);
    }

    KPrintf("\n");

    /* 50mS delay for device*/
    DDL_DelayMS(50UL);

    msg.buf = u8RxBuf;
		
    (void)I2cMasterReadData(i2c_dev1, &msg);

    KPrintf("Master receive data: ");
    
    for (uint16 i = 0; i < msg.len; i++) {
        KPrintf("%d ", (msg.buf)[i]);
    }

    KPrintf("\n");

    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
I2cMasterTest, I2cMasterTest,  i2c master send and receive data);

void I2cSlaveTest(void)
{
    x_err_t ret = EOK;

    struct I2cHardwareDevice *i2c_dev1 = (struct I2cHardwareDevice *)i2c_drv1->driver.owner_bus->owner_haldev;

    struct I2cDataStandard msg;
    uint8 u8RxBuf[256U];

    (void)memset(u8RxBuf, 0, 256U);

    msg.len = 5;
    msg.buf = u8RxBuf;

    KPrintf("I2C receive data\n");

    for (;;) {
        ret = I2cSlaveReadData(i2c_dev1, &msg);
        if (ret != EOK) {
            KPrintf("I2C receive failed!\n");
            break;
        } else {
            KPrintf("Slave receive data: ");
            for (uint16 i = 0; i < msg.len; i++) {
                KPrintf("%d ", (msg.buf)[i]);
            }
            KPrintf("\n");
        }
        
        ret = I2cSlaveWriteData(i2c_dev1, &msg);
        if (ret != EOK) {
            KPrintf("I2C send failed!\n");
            break;
        } else {
            KPrintf("Slave send data: ");
            for (uint16 i = 0; i < msg.len; i++) {
                KPrintf("%d ", (msg.buf)[i]);
            }
            KPrintf("\n");
        }
    }

    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
I2cSlaveTest, I2cSlaveTest,  i2c slave receive and send data);

#endif
