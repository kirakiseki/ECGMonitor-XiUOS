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
* @file connect_spi.c
* @brief support edu-arm32-board spi function and register to bus framework
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-10-17
*/

/*************************************************
File name: connect_spi.c
Description: support edu-arm32-board spi function and register to bus framework
Others: 
History: 
1. Date: 2022-10-17
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board spi configure, write and read
2. support edu-arm32-board spi bus device and driver register
3. SPI1 for LoRa, SPI6 using J12-pin-header to connect 
*************************************************/

#include <connect_spi.h>

#define SPI1_MASTER_SLAVE_MODE           (SPI_MASTER)

/* SPI1 definition */
#define SPI1_UNIT                        (CM_SPI1)
#define SPI1_CLK                         (FCG1_PERIPH_SPI1)

/* SS = PI01 */
#define SPI1_SS_PORT                     (GPIO_PORT_I)
#define SPI1_SS_PIN                      (GPIO_PIN_01)
/* SCK = PH14 */
#define SPI1_SCK_PORT                    (GPIO_PORT_H)
#define SPI1_SCK_PIN                     (GPIO_PIN_14)
#define SPI1_SCK_FUNC                    (GPIO_FUNC_40)
/* MOSI = PI00 */
#define SPI1_MOSI_PORT                   (GPIO_PORT_I)
#define SPI1_MOSI_PIN                    (GPIO_PIN_00)
#define SPI1_MOSI_FUNC                   (GPIO_FUNC_41)
/* MISO = PH15 */
#define SPI1_MISO_PORT                   (GPIO_PORT_H)
#define SPI1_MISO_PIN                    (GPIO_PIN_15)
#define SPI1_MISO_FUNC                   (GPIO_FUNC_42)

#define SPI1_DEVICE_SLAVE_ID_0 0

/* SPI6 definition */
#define SPI6_UNIT                        (CM_SPI6)
#define SPI6_CLK                         (FCG1_PERIPH_SPI6)

/* SS = PE02 */
#define SPI6_SS_PORT                     (GPIO_PORT_E)
#define SPI6_SS_PIN                      (GPIO_PIN_02)
/* SCK = PE03 */
#define SPI6_SCK_PORT                    (GPIO_PORT_E)
#define SPI6_SCK_PIN                     (GPIO_PIN_03)
#define SPI6_SCK_FUNC                    (GPIO_FUNC_46)
/* MOSI = PE04 */
#define SPI6_MOSI_PORT                   (GPIO_PORT_E)
#define SPI6_MOSI_PIN                    (GPIO_PIN_04)
#define SPI6_MOSI_FUNC                   (GPIO_FUNC_47)
/* MISO = PE05 */
#define SPI6_MISO_PORT                   (GPIO_PORT_E)
#define SPI6_MISO_PIN                    (GPIO_PIN_05)
#define SPI6_MISO_FUNC                   (GPIO_FUNC_48)

#define SPI6_DEVICE_SLAVE_ID_0 0

static void HwSpiEnable(CM_SPI_TypeDef *SPIx)
{
    /* Check if the SPI is already enabled */
    if (SPI_CR1_SPE != (SPIx->CR1 & SPI_CR1_SPE)) {
        SPI_Cmd(SPIx, ENABLE);
    }
}

/*Init the spi sdk intetface */
static uint32 SpiSdkInit(struct SpiDriver *spi_drv)
{
    NULL_PARAM_CHECK(spi_drv);

    stc_spi_init_t stcSpiInit;
    stc_gpio_init_t stcGpioInit;

    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_drv->driver.private_data);

#ifdef BSP_USING_SPI1
    /* Configure Port */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI1_SS_PORT, SPI1_SS_PIN, &stcGpioInit);
    GPIO_SetPins(SPI1_SS_PORT, SPI1_SS_PIN);

    GPIO_SetFunc(SPI1_SCK_PORT, SPI1_SCK_PIN, SPI1_SCK_FUNC);
    GPIO_SetFunc(SPI1_MOSI_PORT, SPI1_MOSI_PIN, SPI1_MOSI_FUNC);
    GPIO_SetFunc(SPI1_MISO_PORT, SPI1_MISO_PIN, SPI1_MISO_FUNC);

    /* Configuration SPI */
    FCG_Fcg1PeriphClockCmd(SPI1_CLK, ENABLE);
    SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_4_WIRE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI1_MASTER_SLAVE_MODE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVD;

    if (SPI_MODE_0 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_0;
    } else if (SPI_MODE_1 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_1;
    } else if (SPI_MODE_2 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_2;
    } else if (SPI_MODE_3 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_3;
    }
    
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64;

    if (8 == dev_param->spi_master_param->spi_data_bit_width) {
        stcSpiInit.u32DataBits      = SPI_DATA_SIZE_8BIT;
    } else if (7 == dev_param->spi_master_param->spi_data_bit_width) {
        stcSpiInit.u32DataBits      = SPI_DATA_SIZE_7BIT;
    }
    
    if (SPI_LSB == dev_param->spi_master_param->spi_work_mode & 0x4) {
        stcSpiInit.u32FirstBit      = SPI_FIRST_LSB;
    } else if(SPI_MSB == dev_param->spi_master_param->spi_work_mode & 0x4) {
        stcSpiInit.u32FirstBit      = SPI_FIRST_MSB;
    }

    stcSpiInit.u32FrameLevel        = SPI_1_FRAME;

    (void)SPI_Init(SPI1_UNIT, &stcSpiInit);
    SPI_Cmd(SPI1_UNIT, ENABLE);
#endif

#ifdef BSP_USING_SPI6
    /* Configure Port */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI6_SS_PORT, SPI6_SS_PIN, &stcGpioInit);
    GPIO_SetPins(SPI6_SS_PORT, SPI6_SS_PIN);

    GPIO_SetFunc(SPI6_SCK_PORT, SPI6_SCK_PIN, SPI6_SCK_FUNC);
    GPIO_SetFunc(SPI6_MOSI_PORT, SPI6_MOSI_PIN, SPI6_MOSI_FUNC);
    GPIO_SetFunc(SPI6_MISO_PORT, SPI6_MISO_PIN, SPI6_MISO_FUNC);

    /* Configuration SPI */
    FCG_Fcg1PeriphClockCmd(SPI6_CLK, ENABLE);
    SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_4_WIRE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI1_MASTER_SLAVE_MODE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVD;

    if (SPI_MODE_0 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_0;
    } else if (SPI_MODE_1 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_1;
    } else if (SPI_MODE_2 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_2;
    } else if (SPI_MODE_3 == dev_param->spi_master_param->spi_work_mode & 0x3) {
        stcSpiInit.u32SpiMode       = SPI_MD_3;
    }
    
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64;

    if (8 == dev_param->spi_master_param->spi_data_bit_width) {
        stcSpiInit.u32DataBits      = SPI_DATA_SIZE_8BIT;
    } else if (7 == dev_param->spi_master_param->spi_data_bit_width) {
        stcSpiInit.u32DataBits      = SPI_DATA_SIZE_7BIT;
    }
    
    if (SPI_LSB == dev_param->spi_master_param->spi_work_mode & 0x4) {
        stcSpiInit.u32FirstBit      = SPI_FIRST_LSB;
    } else if(SPI_MSB == dev_param->spi_master_param->spi_work_mode & 0x4) {
        stcSpiInit.u32FirstBit      = SPI_FIRST_MSB;
    }

    stcSpiInit.u32FrameLevel        = SPI_1_FRAME;

    (void)SPI_Init(SPI6_UNIT, &stcSpiInit);
    SPI_Cmd(SPI6_UNIT, ENABLE);
#endif

    return EOK;
}

static uint32 SpiSdkCfg(struct SpiDriver *spi_drv, struct SpiMasterParam *spi_param)
{
    NULL_PARAM_CHECK(spi_drv);
    NULL_PARAM_CHECK(spi_param);  

    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_drv->driver.private_data);

    dev_param->spi_master_param = spi_param;
    dev_param->spi_master_param->spi_work_mode = dev_param->spi_master_param->spi_work_mode & SPI_MODE_MASK;

    return EOK;
}

/*Configure the spi device param, make sure struct (configure_info->private_data) = (SpiMasterParam)*/
static uint32 SpiDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    struct SpiDriver *spi_drv = (struct SpiDriver *)drv;
    struct SpiMasterParam *spi_param;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            ret = SpiSdkInit(spi_drv);
            break;
        case OPE_CFG:
            spi_param = (struct SpiMasterParam *)configure_info->private_data;
            ret = SpiSdkCfg(spi_drv, spi_param);
            break;
        default:
            break;
    }

    return ret;
}

static uint32 SpiWriteData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);

    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    uint8 cs_gpio_port = dev_param->spi_slave_param->spi_cs_gpio_port;
    CM_SPI_TypeDef *spi = spi_dev->haldev.owner_bus->private_data; 

    int i = 0;
    uint32 ret = EOK;
    int32 spi_write_status = 0;

    while (NONE != spi_datacfg) {
        if (spi_datacfg->spi_chip_select) {
            GPIO_ResetPins(cs_gpio_port, cs_gpio_pin);
        }

        if (spi_datacfg->length) {
            uint8 *tx_buff = x_malloc(spi_datacfg->length);
            if ((spi_datacfg->tx_buff) && (tx_buff)) {
                memset(tx_buff, 0, spi_datacfg->length);

                for (i = 0; i < spi_datacfg->length; i++) {
                    tx_buff[i] = ((uint8 *)spi_datacfg->tx_buff)[i];
                }

                HwSpiEnable(spi);
                spi_write_status = SPI_Trans(spi, tx_buff, spi_datacfg->length, 1000);

                while (RESET != SPI_GetStatus(spi, SPI_FLAG_IDLE));

                if (LL_OK == spi_write_status) {
                    ret = ERROR;
                }
            }

            if (tx_buff) {
                x_free(tx_buff);
            }

            SPI_ClearStatus(spi, SPI_FLAG_CLR_ALL | SPI_FLAG_RX_BUF_FULL);            
        }

        if (spi_datacfg->spi_cs_release) {
            GPIO_SetPins(cs_gpio_port, cs_gpio_pin);
        }

        spi_datacfg = spi_datacfg->next;
    }

    return ret;
}

static uint32 SpiReadData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);

    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    uint8 cs_gpio_port = dev_param->spi_slave_param->spi_cs_gpio_port;
    CM_SPI_TypeDef *spi = spi_dev->haldev.owner_bus->private_data; 

    int i = 0;
    int32 spi_read_status = 0;
    uint32 spi_read_length = 0;

    while (NONE != spi_datacfg) {
        if (spi_datacfg->spi_chip_select) {
            GPIO_ResetPins(cs_gpio_port, cs_gpio_pin);
        }

        if (spi_datacfg->length) {
            uint8_t *rx_buff = x_malloc(spi_datacfg->length);
            if ((spi_datacfg->rx_buff) && (rx_buff)) {
                memset(rx_buff, 0xFF, spi_datacfg->length);

                HwSpiEnable(spi);
                spi_read_status = SPI_Receive(spi, rx_buff, spi_datacfg->length, 1000);

                while (RESET != SPI_GetStatus(spi, SPI_FLAG_IDLE));
                if (LL_OK == spi_read_status) {
                    for (i = 0; i < spi_datacfg->length; i++) {           
                        ((uint8_t *)spi_datacfg->rx_buff)[i] = rx_buff[i];
                    }
                }
            }
            if (rx_buff) {
                x_free(rx_buff);
            }

            SPI_ClearStatus(spi, SPI_FLAG_CLR_ALL | SPI_FLAG_RX_BUF_FULL);
        }

        if (spi_datacfg->spi_cs_release) {
            GPIO_SetPins(cs_gpio_port, cs_gpio_pin);
        }

        spi_read_length += spi_datacfg->length;
        spi_datacfg = spi_datacfg->next;
    }

    return spi_read_length;
}

/*manage the spi device operations*/
static const struct SpiDevDone spi_dev_done =
{
    .dev_open = NONE,
    .dev_close = NONE,
    .dev_write = SpiWriteData,
    .dev_read = SpiReadData,
};

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

/*Attach the spi device to the spi bus*/
static int BoardSpiDevBend(void)
{
    x_err_t ret = EOK;

#ifdef SPI_1_DEVICE_NAME_0
    static struct SpiHardwareDevice spi1_device0;
    memset(&spi1_device0, 0, sizeof(struct SpiHardwareDevice));

    static struct SpiSlaveParam spi1_slaveparam0;
    memset(&spi1_slaveparam0, 0, sizeof(struct SpiSlaveParam));

    spi1_slaveparam0.spi_slave_id = SPI1_DEVICE_SLAVE_ID_0;
    spi1_slaveparam0.spi_cs_gpio_pin = SPI1_SS_PIN;
    spi1_slaveparam0.spi_cs_gpio_port = SPI1_SS_PORT;

    spi1_device0.spi_param.spi_slave_param = &spi1_slaveparam0;

    spi1_device0.spi_dev_done = &(spi_dev_done);

    ret = SpiDeviceRegister(&spi1_device0, (void *)(&spi1_device0.spi_param), SPI_1_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceRegister device %s error %d\n", SPI_1_DEVICE_NAME_0, ret);
        return ERROR;
    }  

    ret = SpiDeviceAttachToBus(SPI_1_DEVICE_NAME_0, SPI_BUS_NAME_1);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceAttachToBus device %s error %d\n", SPI_1_DEVICE_NAME_0, ret);
        return ERROR;
    }  
#endif

#ifdef SPI_6_DEVICE_NAME_0
    static struct SpiHardwareDevice spi6_device0;
    memset(&spi6_device0, 0, sizeof(struct SpiHardwareDevice));

    static struct SpiSlaveParam spi6_slaveparam0;
    memset(&spi6_slaveparam0, 0, sizeof(struct SpiSlaveParam));

    spi6_slaveparam0.spi_slave_id = SPI1_DEVICE_SLAVE_ID_0;
    spi6_slaveparam0.spi_cs_gpio_pin = SPI6_SS_PIN;
    spi6_slaveparam0.spi_cs_gpio_port = SPI6_SS_PORT;

    spi6_device0.spi_param.spi_slave_param = &spi6_slaveparam0;

    spi6_device0.spi_dev_done = &(spi_dev_done);

    ret = SpiDeviceRegister(&spi6_device0, (void *)(&spi6_device0.spi_param), SPI_6_DEVICE_NAME_0);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceRegister device %s error %d\n", SPI_6_DEVICE_NAME_0, ret);
        return ERROR;
    }  

    ret = SpiDeviceAttachToBus(SPI_6_DEVICE_NAME_0, SPI_BUS_NAME_6);
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend SpiDeviceAttachToBus device %s error %d\n", SPI_6_DEVICE_NAME_0, ret);
        return ERROR;
    }  
#endif

    return ret;
}

int HwSpiInit(void)
{
    x_err_t ret = EOK;

#ifdef BSP_USING_SPI1
    static struct SpiBus spi1_bus;
    memset(&spi1_bus, 0, sizeof(struct SpiBus));

    static struct SpiDriver spi1_driver;
    memset(&spi1_driver, 0, sizeof(struct SpiDriver));

    spi1_bus.private_data = SPI1_UNIT;
    spi1_driver.configure = SpiDrvConfigure;

    ret = BoardSpiBusInit(&spi1_bus, &spi1_driver, SPI_BUS_NAME_1, SPI_1_DRV_NAME);
    if (EOK != ret) {
        KPrintf("BoardSpiBusInit error ret %u\n", ret);
        return ERROR;
    }

#endif

#ifdef BSP_USING_SPI6
    static struct SpiBus spi6_bus;
    memset(&spi6_bus, 0, sizeof(struct SpiBus));

    static struct SpiDriver spi6_driver;
    memset(&spi6_driver, 0, sizeof(struct SpiDriver));

    spi6_bus.private_data = SPI6_UNIT;
    spi6_driver.configure = SpiDrvConfigure;

    ret = BoardSpiBusInit(&spi6_bus, &spi6_driver, SPI_BUS_NAME_6, SPI_6_DRV_NAME);
    if (EOK != ret) {
        KPrintf("BoardSpiBusInit error ret %u\n", ret);
        return ERROR;
    }

#endif

    ret = BoardSpiDevBend();
    if (EOK != ret) {
        KPrintf("BoardSpiDevBend error ret %u\n", ret);
        return ERROR;
    } 

    return ret;
}

/*Just for lora test*/
static struct Bus *bus;
static struct HardwareDev *dev;
static struct Driver *drv;

static uint32 TestSpiLoraOpen(void)
{
    NULL_PARAM_CHECK(drv);

    KPrintf("SpiLoraOpen start\n");

    x_err_t ret = EOK;

    struct BusConfigureInfo configure_info;
    struct SpiMasterParam spi_master_param;
    spi_master_param.spi_data_bit_width = 8;
    spi_master_param.spi_work_mode = SPI_MODE_0 | SPI_MSB;

    configure_info.configure_cmd = OPE_CFG;
    configure_info.private_data = (void *)&spi_master_param;
    ret = BusDrvConfigure(drv, &configure_info);
    if (ret) {
        KPrintf("spi drv OPE_CFG error drv %8p cfg %8p\n", drv, &spi_master_param);
        return ERROR;
    }

    configure_info.configure_cmd = OPE_INT;
    ret = BusDrvConfigure(drv, &configure_info);
    if (ret) {
        KPrintf("spi drv OPE_INT error drv %8p\n", drv);
        return ERROR;
    }
    
    return ret;
}

static void TestSpiRead(void)
{
    struct BusBlockWriteParam write_param;
    struct BusBlockReadParam read_param;

    uint8 write_addr = 0x06 & 0x7F;
    uint8 read_data = 0;

    BusDevOpen(dev);

    write_param.buffer = (void *)&write_addr;
    write_param.size = 1;
    BusDevWriteData(dev, &write_param);

    read_param.buffer = (void *)&read_data;
    read_param.size = 1;
    BusDevReadData(dev, &read_param);

    BusDevClose(dev);

    KPrintf("read data from lora 0x06 register, receive data 0x%x\n", read_data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                                                TestSpiRead, TestSpiRead, read data from lora register);

void TestLoraOpen(void)
{
    x_err_t ret = EOK;

    bus = BusFind(SPI_BUS_NAME_1);
    dev = BusFindDevice(bus, SPI_1_DEVICE_NAME_0);
    drv = BusFindDriver(bus, SPI_1_DRV_NAME);

    bus->match(drv, dev);
 
    ret = TestSpiLoraOpen();
    if (EOK != ret) {
        KPrintf("LoRa init failed\n");
        return;
    }

    KPrintf("LoRa init succeed\n");

    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                                                TestLoraOpen, TestLoraOpen, open lora device and read parameters);


