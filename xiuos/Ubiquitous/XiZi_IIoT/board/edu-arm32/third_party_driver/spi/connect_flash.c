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
* @file connect_flash.c
* @brief support edu-arm32-board qspi-flash function and register to bus framework
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-02-16
*/
#include <connect_flash.h>

#define QSPI_DEVICE_SLAVE_ID_0 (0)
#define QSPI_UNIT    (CM_QSPI)

#define QSPI_CS_PORT (GPIO_PORT_C)
#define QSPI_SCK_PORT (GPIO_PORT_C)
#define QSPI_IO0_PORT (GPIO_PORT_D)
#define QSPI_IO1_PORT (GPIO_PORT_D)
#define QSPI_IO2_PORT (GPIO_PORT_D)
#define QSPI_IO3_PORT (GPIO_PORT_D)

#define QSPI_CS_PIN (GPIO_PIN_07)
#define QSPI_SCK_PIN (GPIO_PIN_06)
#define QSPI_IO0_PIN (GPIO_PIN_08)
#define QSPI_IO1_PIN (GPIO_PIN_09)
#define QSPI_IO2_PIN (GPIO_PIN_10)
#define QSPI_IO3_PIN (GPIO_PIN_11)

#define QSPI_PIN_FUNC (GPIO_FUNC_18)

static uint32 QSpiSdkInit(struct SpiDriver *spi_drv)
{
    stc_qspi_init_t stcInit;

    FCG_Fcg1PeriphClockCmd(PWC_FCG1_QSPI, ENABLE);

    (void)QSPI_StructInit(&stcInit);
    stcInit.u32ClockDiv      = QSPI_CLK_DIV3;
    stcInit.u32SpiMode       = QSPI_SPI_MD0;
    stcInit.u32ReadMode      = QSPI_RD_MD_STD_RD;
    stcInit.u32DummyCycle   = QSPI_DUMMY_CYCLE8;        
    stcInit.u32AddrWidth     = QSPI_ADDR_WIDTH_24BIT;
    return QSPI_Init(&stcInit);
    
}

static void QspiPinConfig(void)
{
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(QSPI_CS_PORT, QSPI_CS_PIN|QSPI_SCK_PIN, &stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_SET;
    (void)GPIO_Init(QSPI_IO0_PORT, QSPI_IO1_PIN|QSPI_IO2_PIN|QSPI_IO3_PIN, &stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(QSPI_IO0_PORT, QSPI_IO0_PIN, &stcGpioInit);

    GPIO_ResetPins(QSPI_CS_PORT, QSPI_CS_PIN);
    GPIO_SetPins(QSPI_IO0_PORT, QSPI_IO2_PIN|QSPI_IO3_PIN);

    GPIO_SetFunc(QSPI_CS_PORT, QSPI_CS_PIN, QSPI_PIN_FUNC);
    GPIO_SetFunc(QSPI_SCK_PORT, QSPI_SCK_PIN, QSPI_PIN_FUNC);
    GPIO_SetFunc(QSPI_IO0_PORT, QSPI_IO0_PIN, QSPI_PIN_FUNC);
    GPIO_SetFunc(QSPI_IO1_PORT, QSPI_IO1_PIN, QSPI_PIN_FUNC);
    GPIO_SetFunc(QSPI_IO2_PORT, QSPI_IO2_PIN, QSPI_PIN_FUNC);
    GPIO_SetFunc(QSPI_IO3_PORT, QSPI_IO3_PIN, QSPI_PIN_FUNC);
}

static uint32 QSpiWriteData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);
    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    uint8 cs_gpio_port = dev_param->spi_slave_param->spi_cs_gpio_port;
    CM_SPI_TypeDef *spi = spi_dev->haldev.owner_bus->private_data; 
    x_err_t ret = EOK;
    if (spi_datacfg->spi_chip_select) {
        // GPIO_ResetPins(cs_gpio_port, cs_gpio_pin);
        QSPI_EnterDirectCommMode();
    }
    if(spi_datacfg->length > 0U && spi_datacfg->tx_buff!=NULL){
        for(int i=0;i<spi_datacfg->length;i++){
            QSPI_WriteDirectCommValue(spi_datacfg->tx_buff[i]);           
        }
    }
    if (spi_datacfg->spi_cs_release) {
        // GPIO_SetPins(cs_gpio_port, cs_gpio_pin);
        QSPI_ExitDirectCommMode();   
    }
    return ret;
}

static uint32 QSpiReadData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);
    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    uint8 cs_gpio_port = dev_param->spi_slave_param->spi_cs_gpio_port;
    CM_SPI_TypeDef *spi = spi_dev->haldev.owner_bus->private_data; 
    x_err_t ret = EOK;
    uint8_t *read_buffer = spi_datacfg->rx_buff;

    if (spi_datacfg->spi_chip_select) {
        // GPIO_ResetPins(cs_gpio_port, cs_gpio_pin);
        QSPI_EnterDirectCommMode();
    }
    if(spi_datacfg->length > 0U && spi_datacfg->rx_buff!=NULL){
        for(int i=0;i<spi_datacfg->length;i++){
            read_buffer[i] = (uint8_t)QSPI_ReadDirectCommValue();            
        }
    }
    if (spi_datacfg->spi_cs_release) {
        // GPIO_SetPins(cs_gpio_port, cs_gpio_pin);
        QSPI_ExitDirectCommMode(); 
    }
    return ret;
}

static uint32 QSpiDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    struct SpiDriver *spi_drv = (struct SpiDriver *)drv;
    struct SpiMasterParam *spi_param;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            QSpiSdkInit(spi_drv);
            QspiPinConfig();
            break;
        case OPE_CFG:
            spi_param = (struct SpiMasterParam *)configure_info->private_data;
            break;
        default:
            break;
    }

    return ret;
}

/*manage the qspi device operations*/
static const struct SpiDevDone qspi_dev_done =
{
    .dev_open = NONE,
    .dev_close = NONE,
    .dev_write = QSpiWriteData,
    .dev_read = QSpiReadData,
};

static int BoardQSpiDevBend(void)
{
    x_err_t ret = EOK;

    static struct SpiHardwareDevice qspi_device0;
    memset(&qspi_device0, 0, sizeof(struct SpiHardwareDevice));

    static struct SpiSlaveParam qspi_slaveparam0;
    memset(&qspi_slaveparam0, 0, sizeof(struct SpiSlaveParam));

    qspi_slaveparam0.spi_slave_id = QSPI_DEVICE_SLAVE_ID_0;
    qspi_slaveparam0.spi_cs_gpio_pin = QSPI_CS_PIN;
    qspi_slaveparam0.spi_cs_gpio_port = QSPI_CS_PORT;

    qspi_device0.spi_param.spi_slave_param = &qspi_slaveparam0;
    qspi_device0.spi_dev_done = &(qspi_dev_done);

    ret = SpiDeviceRegister(&qspi_device0, (void *)(&qspi_device0.spi_param), QSPI_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceRegister device %s error %d\n", QSPI_DEVICE_NAME_0, ret);
        return ERROR;
    }  

    ret = SpiDeviceAttachToBus(QSPI_DEVICE_NAME_0, QSPI_BUS_NAME);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceAttachToBus device %s error %d\n", QSPI_DEVICE_NAME_0, ret);
        return ERROR;
    }

    return ret;
}

static int BoardSpiBusInit(struct SpiBus *spi_bus, struct SpiDriver *spi_driver, const char *bus_name, const char *drv_name)
{
    x_err_t ret = EOK;

    /*Init the spi bus */
    ret = SpiBusInit(spi_bus, bus_name);
    if (EOK != ret) {
        KPrintf("Board_Spi_init SpiBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the spi driver*/
    ret = SpiDriverInit(spi_driver, drv_name);
    if (EOK != ret) {
        KPrintf("Board_Spi_init SpiDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the spi driver to the spi bus*/
    ret = SpiDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("Board_Spi_init SpiDriverAttachToBus error %d\n", ret);
        return ERROR;
    } 

    return ret;
}


int HwQSpiInit(void)
{
    x_err_t ret = EOK;

    static struct SpiBus qspi_bus;
    memset(&qspi_bus, 0, sizeof(struct SpiBus));

    static struct SpiDriver qspi_driver;
    memset(&qspi_driver, 0, sizeof(struct SpiDriver));

    qspi_bus.private_data = QSPI_UNIT;
    qspi_driver.configure = QSpiDrvConfigure;

    ret = BoardSpiBusInit(&qspi_bus, &qspi_driver, QSPI_BUS_NAME, QSPI_DRV_NAME);
    if (EOK != ret) {
        KPrintf("BoardSpiBusInit error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardQSpiDevBend();
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend error ret %u\n", ret);
        return ERROR;
    } 

    return ret;
}

int FlashW25qxxSpiDeviceInit(void)
{
    HwQSpiInit();
    QSpiSdkInit(NULL);
    QspiPinConfig();
    if (NONE == SpiFlashInit(QSPI_BUS_NAME, QSPI_DEVICE_NAME_0, QSPI_DRV_NAME, QSPI_FLASH_DEV_NAME)) {
        return ERROR;
    }
    return EOK;
}
