#include <xizi.h>
#include <device.h>
#include <fpioa.h>
#include <gpiohs.h>
#include "drv_io_config.h"
#include <plic.h>
#include <utils.h>
#include <connect_soft_spi.h>
#include <sleep.h>

#include <sd_spi.h>
#include <dev_spi.h>
#include <bus_spi.h>

static x_err_t SoftSPIinit(struct SpiDriver *spi_drv, struct BusConfigureInfo *cfg)
{
    NULL_PARAM_CHECK(spi_drv);
    NULL_PARAM_CHECK(cfg);

    // mode  CPOL = 0  CPHA = 0
    gpiohs_set_drive_mode(SOFT_SPI_CS0_PIN, GPIO_DM_OUTPUT);
    gpiohs_set_pin(SOFT_SPI_CS0_PIN, GPIO_PV_HIGH); // set the cs gpio high
    gpiohs_set_drive_mode(SOFT_SPI_SCK, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(SOFT_SPI_MOSI, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(SOFT_SPI_MISO, GPIO_DM_INPUT);
    gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_LOW);
    KPrintf("%s init done\n", SOFT_SPI_BUS_NAME);

    return EOK;
}

static uint32 SoftSpiDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    struct SpiDriver *spi_drv = (struct SpiDriver *)drv;
    struct SpiMasterParam *spi_param;

    switch (configure_info->configure_cmd)
    {
    case OPE_INT:
        SoftSPIinit(spi_drv, configure_info);
        break;

    case OPE_CFG:
        break;
    default:

        break;
    }

    return ret;
}

static void SoftSpiWriteByte(struct SpiHardwareDevice *spi_dev, uint8_t data)
{
    int8_t i = 0;
    uint8_t temp = 0;
    for (i = 0; i < 8; i++)
    {
        temp = ((data & 0x80) == 0x80) ? 1 : 0;
        data = data << 1;
        gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_LOW);
        usleep(SOFT_SPI_CLK_DELAY);
        if (0 == temp)
        {
            gpiohs_set_pin(SOFT_SPI_MOSI, GPIO_PV_LOW);
        }
        else
        {
            gpiohs_set_pin(SOFT_SPI_MOSI, GPIO_PV_HIGH);
        }
        gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_HIGH);
        usleep(SOFT_SPI_CLK_DELAY);
    }
    gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_LOW);
}

/* 读一个字节 */
static uint8_t SoftSpiReadbyte(struct SpiHardwareDevice *spi_dev)
{
    uint8_t i = 0;
    uint8_t read_data = 0xFF;
    for (i = 0; i < 8; i++)
    {
        read_data = read_data << 1;
        gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_LOW);
        usleep(SOFT_SPI_CLK_DELAY);
        gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_HIGH);
        usleep(SOFT_SPI_CLK_DELAY);
        if (1 == gpiohs_get_pin(SOFT_SPI_MISO))
        {
            read_data = read_data | 0x01;
        }
    }
    return read_data;
}

/* 读写一个字节 */
// this funcition is unverify until now!
static uint8_t SoftSpiReadWriteByte(struct SpiHardwareDevice *spi_dev, uint8_t data)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t read_data = 0xFF;
    for (i = 0; i < 8; i++)
    {
        temp = ((data & 0x80) == 0x80) ? 1 : 0;
        data = data << 1;
        read_data = read_data << 1;
        if (temp == 0)
        {
            gpiohs_set_pin(SOFT_SPI_MOSI, GPIO_PV_LOW);
        }
        else
        {
            gpiohs_set_pin(SOFT_SPI_MOSI, GPIO_PV_HIGH);
        }
        usleep(SOFT_SPI_CLK_DELAY);
        gpiohs_set_pin(SOFT_SPI_SCK, GPIO_PV_HIGH);
        usleep(SOFT_SPI_CLK_DELAY);
        if (gpiohs_get_pin(SOFT_SPI_MISO) == 1)
        {
            read_data = read_data + 1;
        }
    }
    return read_data;
}

static uint32 SoftSpiWriteData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);

    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    const uint8_t *data_buff = spi_datacfg->tx_buff;
    int data_length = spi_datacfg->length;
    if (NONE == spi_datacfg->tx_buff)
    {
        data_length = 0;
    }

    if (spi_datacfg->spi_chip_select)
    {
        gpiohs_set_pin(cs_gpio_pin, GPIO_PV_LOW);
    }

    for (size_t i = 0; i < data_length; i++)
    {
        SoftSpiWriteByte(spi_dev, data_buff[i]);
    }

    if (spi_datacfg->spi_cs_release)
    {
        gpiohs_set_pin(cs_gpio_pin, GPIO_PV_HIGH);
    }
    spi_datacfg = spi_datacfg->next;

    return EOK;
}

static uint32 SoftSpiReadData(struct SpiHardwareDevice *spi_dev, struct SpiDataStandard *spi_datacfg)
{
    SpiDeviceParam *dev_param = (SpiDeviceParam *)(spi_dev->haldev.private_data);
    uint8 cs_gpio_pin = dev_param->spi_slave_param->spi_cs_gpio_pin;
    uint8_t *recv_buff = spi_datacfg->rx_buff;
    int recv_length = spi_datacfg->length;

    if (NONE == spi_datacfg->rx_buff)
    {
        recv_length = 0;
    }

    if (spi_datacfg->spi_chip_select)
    {
        gpiohs_set_pin(cs_gpio_pin, GPIO_PV_LOW);
    }

    for (size_t i = 0; i < recv_length; i++)
    {
        recv_buff[i] = SoftSpiReadbyte(spi_dev);
    }

    if (spi_datacfg->spi_cs_release)
    {
        gpiohs_set_pin(cs_gpio_pin, GPIO_PV_HIGH);
    }

    spi_datacfg = spi_datacfg->next;

    return spi_datacfg->length;
}

const struct SpiDevDone soft_spi_dev_done = {
    .dev_close = NONE,
    .dev_open = NONE,
    .dev_read = SoftSpiReadData,
    .dev_write = SoftSpiWriteData};

static int BoardSoftSpiBusInit(struct SpiBus *spi_bus, struct SpiDriver *spi_driver)
{
    x_err_t ret = EOK;

    /*Init the spi bus */
    ret = SpiBusInit(spi_bus, SOFT_SPI_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_init SpiBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the spi driver*/
    ret = SpiDriverInit(spi_driver, SOFT_SPI_DRV_NAME);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_init SpiDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the spi driver to the spi bus*/
    ret = SpiDriverAttachToBus(SOFT_SPI_DRV_NAME, SOFT_SPI_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_init SpiDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

static int BoardSoftSpiDevBend(void)
{
    x_err_t ret = EOK;

    static struct SpiHardwareDevice spi_device0;
    memset(&spi_device0, 0, sizeof(struct SpiHardwareDevice));

    static struct SpiSlaveParam spi_slaveparam0;
    memset(&spi_slaveparam0, 0, sizeof(struct SpiSlaveParam));

    spi_slaveparam0.spi_slave_id = SOFT_SPI_DEVICE_SLAVE_ID;
    spi_slaveparam0.spi_cs_gpio_pin = SOFT_SPI_CS0_PIN;
    spi_slaveparam0.spi_cs_select_id = SOFT_SPI_CHIP_SELECT;

    spi_device0.spi_param.spi_dma_param = NONE;
    spi_device0.spi_param.spi_slave_param = &spi_slaveparam0;

    spi_device0.spi_dev_done = &(soft_spi_dev_done);

    ret = SpiDeviceRegister(&spi_device0, (void *)(&spi_device0.spi_param), SOFT_SPI_DEVICE_NAME);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_init SpiDeviceInit device %s error %d\n", SOFT_SPI_DEVICE_NAME, ret);
        return ERROR;
    }

    ret = SpiDeviceAttachToBus(SOFT_SPI_DEVICE_NAME, SOFT_SPI_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_init SpiDeviceAttachToBus device %s error %d\n", SOFT_SPI_DEVICE_NAME, ret);
        return ERROR;
    }

    return ret;
}

int HwSoftSPIInit(void)
{
    x_err_t ret = EOK;

    static struct SpiBus spi_bus;
    memset(&spi_bus, 0, sizeof(struct SpiBus));

    static struct SpiDriver spi_driver;
    memset(&spi_driver, 0, sizeof(struct SpiDriver));

    spi_driver.configure = &(SoftSpiDrvConfigure);

    ret = BoardSoftSpiBusInit(&spi_bus, &spi_driver);
    if (EOK != ret)
    {
        KPrintf("Board_Spi_Init error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardSoftSpiDevBend();
    if (EOK != ret)
    {
        KPrintf("Board_Spi_Init error ret %u\n", ret);
        return ERROR;
    }

    return ret;
}
