#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_sx1278.h"
#include <dfs_file.h>
#include <radio.h>
#include "spi_lora_sx12xx.h"
#include "sx1276-Hal.h"
#include "sx1276-Lora.h"
#include "sx1276-LoraMisc.h"
#include <sx1276.h>

#include <stdio.h>
#ifdef RT_USING_POSIX
#include <dfs_posix.h>
#include <dfs_poll.h>
#endif
#ifdef DRV_USING_SX1278

#define SX12XX_DO0_PIN  GET_PIN(C, 1)
#define SX12XX_DO1_PIN  GET_PIN(C, 0)
#define SX12XX_RST_PIN GET_PIN(F,11)
static rt_bool_t init_flag = RT_FALSE;
static rt_bool_t enable_master = RT_FALSE;
tRadioDriver *Radio = RT_NULL;
struct lora_device
{
    struct rt_device parent;
};
struct lora_device sx1278_device;
static struct rt_spi_device *spi_bus;


void SX1276InitIo(void)
{
    rt_pin_mode(SX12XX_DO0_PIN, PIN_MODE_INPUT);
    rt_pin_mode(SX12XX_DO1_PIN, PIN_MODE_INPUT);
}

void SX1276SetReset(rt_uint8_t state)
{
    if (state == RADIO_RESET_ON)
    {
        rt_pin_mode(SX12XX_RST_PIN, PIN_MODE_OUTPUT);
        rt_pin_write(SX12XX_RST_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_mode(SX12XX_RST_PIN, PIN_MODE_INPUT);

    }
}
inline rt_uint8_t SX1276ReadDio0(void)
{
    return rt_pin_read(SX12XX_DO0_PIN);
}

inline rt_uint8_t SX1276ReadDio1(void)
{
    return rt_pin_read(SX12XX_DO1_PIN);
}

inline rt_uint8_t SX1276ReadDio2(void)
{
    return rt_pin_read(SX1278_DEVICE_DIO2_PIN);
}

inline rt_uint8_t SX1276ReadDio3(void)
{
    return rt_pin_read(SX1278_DEVICE_DIO3_PIN);
}
inline rt_uint8_t SX1276ReadDio4(void)
{
    return rt_pin_read(SX1278_DEVICE_DIO4_PIN);
}
inline rt_uint8_t SX1276ReadDio5(void)
{
    return rt_pin_read(SX1278_DEVICE_DIO5_PIN);
}
inline void SX1276WriteRxTx(rt_uint8_t txEnable)
{
    if (txEnable != 0)
    {

    }
    else
    {

    }
}


void SX1276Write(rt_uint8_t addr, rt_uint8_t data)
{
    SX1276WriteBuffer(addr, &data, 1);
}

void SX1276Read(rt_uint8_t addr, rt_uint8_t *data)
{
    SX1276ReadBuffer(addr, data, 1);
}

void SX1276WriteBuffer(rt_uint8_t addr, rt_uint8_t *buffer, rt_uint8_t size)
{
    spi_bus = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    struct rt_spi_message msg1, msg2;
    rt_uint8_t    data = (addr | 0x80);
    msg1.send_buf   = &data;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 1;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = &msg2;

    msg2.send_buf   = buffer;
    msg2.recv_buf   = RT_NULL;
    msg2.length     = size;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;
    rt_spi_transfer_message(spi_bus, &msg1);
}

void SX1276ReadBuffer(rt_uint8_t addr, rt_uint8_t *buffer, rt_uint8_t size)
{
    spi_bus = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    struct rt_spi_message msg1, msg2;
    rt_uint8_t    data = (addr & 0x7F);
    msg1.send_buf   = &data;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 1;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = &msg2;

    msg2.send_buf   = RT_NULL;
    msg2.recv_buf   = buffer;
    msg2.length     = size;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;
    rt_spi_transfer_message(spi_bus, &msg1);
}

void SX1276WriteFifo(rt_uint8_t *buffer, rt_uint8_t size)
{
    SX1276WriteBuffer(0, buffer, size);
}

void SX1276ReadFifo(rt_uint8_t *buffer, rt_uint8_t size)
{
    SX1276ReadBuffer(0, buffer, size);
}

struct rt_spi_device *sx12xx_init(const char *dev_name, rt_uint8_t param)
{
    RT_ASSERT(dev_name);
    spi_bus = (struct rt_spi_device *)rt_device_find(dev_name);
    if (!spi_bus)
    {
        rt_kprintf("spi sample run failed! cant't find %s device!\n", dev_name);
        return RT_NULL;
    }
    /* config spi */
    else
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0. */
        cfg.max_hz = 10 * 1000000;             /* max 10M */
        rt_spi_configure(spi_bus, &cfg);
    }
	Radio = RadioDriverInit();
    Radio->Init();	
    return spi_bus;
}

/*
 * This function supports to read data from the lora
 */
static rt_size_t lora_sx1278_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
    RT_ASSERT(dev!=RT_NULL);
    int read_times = 100;
    SX1276StartRx();
    rt_kprintf("read lora ready!\n");

    while (read_times)
    {
        if(SX1276Process()!=RF_RX_DONE)
        {
            read_times --;
            rt_thread_mdelay(500);
        }
        else
        {
            break;
        }
    }
    if(read_times>0)
    {
        SX1276GetRxPacket(buffer,(uint16_t *)&size);
    }
    else
    {
        size = 0;
    }


    return size;

}

/*
 * This function supports to write data to the lora
 */
static rt_size_t lora_sx1278_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    RT_ASSERT(dev !=RT_NULL);
    if(size > 256)
    {
        rt_kprintf("error:message is too long!\n");
        return RT_ERROR;
    }
    else
    {
        SX1276SetTxPacket(buffer,size);
        while(SX1276Process() != RF_TX_DONE);
        rt_kprintf("write success!\n");
    }


    return RT_EOK;
}

static int sx1278_test(int argc,char **argv)
{
    char name[RT_NAME_MAX];
    if(argc ==2)
    {
        rt_strncpy(name,argv[1],RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(name,"spi20",RT_NAME_MAX);
    }
    if(spi_bus == RT_NULL)
    {
        spi_bus = sx12xx_init("spi20",RT_NULL);
        if(spi_bus == RT_NULL)
        {
            rt_kprintf("sx12xx init failed\n");
            return -1;
        }
    }
    rt_kprintf("sx12xx init succeed\n");
    if (0x91 != SX1276_Spi_Check())
    {
        rt_kprintf("sx12xx spi check failed!\n!");
    }
    else
    {
        rt_kprintf("sx12xx spi check ok!\n");
    }
    return 0;
}
MSH_CMD_EXPORT(sx1278_test,SAMPLE);

rt_uint8_t SX1276_Spi_Check()
{
    uint8_t test = 0;
	tLoRaSettings settings;
    spi_bus = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    SX1276Read(REG_LR_VERSION,&test);
	rt_kprintf("version code of the chip is %x\n",test);
	settings.RFFrequency = SX1276LoRaGetRFFrequency();
	rt_kprintf("SX1278 Lora parameters are :\nRFFrequency is %d\n",settings.RFFrequency);
	settings.Power = SX1276LoRaGetRFPower();
	rt_kprintf("RFPower is %d\n",settings.Power);
	settings.SignalBw = SX1276LoRaGetSignalBandwidth();	  
	rt_kprintf("SignalBw is %d\n",settings.SignalBw);
	settings.SpreadingFactor = SX1276LoRaGetSpreadingFactor();
	rt_kprintf("SpreadingFactor is %d\n",settings.SpreadingFactor);
    SX1276Write(REG_LR_HOPPERIOD, 0x91); //选一个用不到的寄存器来做验证
    SX1276Read(REG_LR_HOPPERIOD, &test);
    if (test != 0x91)
    {
        return -1;
    }		
    return test;
}

static rt_uint16_t ComputerCrc (rt_uint8_t *data,rt_uint16_t length)
{
    int i;
    rt_uint16_t crc ;
    while (length --)
    {
        crc ^= *data++;
        for(i =0;i<8;i++)
        {
            if(crc & 0x01)
                crc = crc >>1^ 0xA001;
            else
                crc = crc >>1;
        }
    }
    return crc;
}

static rt_err_t CrcCheck(rt_uint8_t *data,rt_uint16_t length)
{
    rt_uint16_t crc_data;
    rt_uint16_t input_data ;
    input_data = (((rt_uint16_t)data[length - 1] << 8) & 0xFF00) | ((rt_uint16_t)data[length - 2] & 0x00FF);
    crc_data  = ComputerCrc(data,length-2);

    if(crc_data == input_data)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static int sx1278_fops_open(struct dfs_fd *fd)
{
    rt_err_t ret;
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev !=RT_NULL);
    ret = rt_device_open(dev,RT_DEVICE_FLAG_RDONLY);
    if(ret ==RT_EOK)
    {
        return RT_EOK;
    }
    spi_bus = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    if(!spi_bus)
    {
        rt_kprintf("can not find spi bus\n");
        return -RT_ERROR;
    }

    struct rt_spi_configuration cfg;
    cfg.data_width = 8 ;
    cfg.max_hz     =  10* 1000000;
    cfg.mode       =  RT_SPI_MODE_0 |RT_SPI_MSB;
    rt_spi_configure(spi_bus,&cfg);
    Radio = RadioDriverInit();

    Radio->Init();
}

static int sx1278_fops_close(struct dfs_fd *fd)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    rt_device_set_rx_indicate(dev,RT_NULL);
    rt_device_close(dev);
    return RT_EOK;
}

static int sx1278_fops_read(struct dfs_fd *fd,void *buf,rt_size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev!=RT_NULL);
    rt_err_t ret;
    ret = lora_sx1278_read(dev,0,buf,count);
    if(ret < 0 )
    {
        return -RT_ERROR;
    }
}

static int sx1278_fops_write(struct dfs_fd *fd,void *buf,rt_size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev!=RT_NULL);
    rt_err_t ret ;

    ret = lora_sx1278_write(dev,0,buf,count);
    if(ret<0)
    {
        return -RT_ERROR;
    }
}

static rt_err_t lora_sx1278_open(rt_device_t dev,rt_uint16_t oflag)
{
    RT_ASSERT(dev !=RT_NULL);
    return RT_EOK;
}

static rt_err_t lora_sx1278_close(rt_device_t dev)
{
    RT_ASSERT(dev !=RT_NULL);
    return RT_EOK;
}

static rt_err_t  lora_sx1278_init(rt_device_t dev)
{
    RT_ASSERT(dev!=RT_NULL);
    spi_bus = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    if(!spi_bus)
    {
        rt_kprintf("can not find spi bus\n");
        return -RT_ERROR;
    }

    struct rt_spi_configuration cfg;
    cfg.data_width = 8 ;
    cfg.max_hz     =  10* 1000000;
    cfg.mode       =  RT_SPI_MODE_0 |RT_SPI_MSB;
    rt_spi_configure(spi_bus,&cfg);
    Radio = RadioDriverInit();

    Radio->Init();
    return RT_EOK;
}

/* ------------------test--------------------*/
static rt_err_t LoraSend(int argc,char**argv)
{
    rt_device_t dev;
    struct BlockWriteParam write_param;
    dev = rt_device_find("sx1278");
    int result;
    int len;
    char msg[LORA_BUFFER_SIZE] = {0};
    if(argc<2)
    {
        rt_kprintf("Usage: %s <string>\n",argv[0]);
        return -RT_ERROR;
    }
    rt_strncpy(msg,argv[1],LORA_BUFFER_SIZE);
    len = strlen(msg);
    write_param.buffer = msg;
    write_param.size = rt_strlen(msg);

    result = lora_sx1278_write(dev,0,write_param.buffer,write_param.size);
    rt_memset(&write_param,0,sizeof(struct BlockWriteParam));
    if(result < 0)
    {
        rt_kprintf("send error!\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(LoraSend,lora send sample);

static rt_err_t LoraReceive(void)
{
    char sx1278_recv_msg[256];
    int ret;
    rt_device_t dev;
    dev = rt_device_find("sx1278");
    while(1)
    {
    ret = lora_sx1278_read(dev,0,sx1278_recv_msg,256);
    if(ret<0)
    {
        rt_kprintf("receive error!\n");
        return -RT_ERROR;
    }
    rt_memset(sx1278_recv_msg,0,256);
    return RT_EOK;
    }
}
MSH_CMD_EXPORT(LoraReceive,lora receive sample);

const static struct dfs_file_ops sx1278_fops = 
{
    sx1278_fops_open,
    sx1278_fops_close,
    RT_NULL,
    sx1278_fops_read,
    sx1278_fops_write,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
};


int sx1278_init()
{
    rt_err_t ret = RT_EOK;
    sx1278_device.parent.open = lora_sx1278_open;
    sx1278_device.parent.close = lora_sx1278_close;
    sx1278_device.parent.read = lora_sx1278_read;
    sx1278_device.parent.write = lora_sx1278_write;
    sx1278_device.parent.init = lora_sx1278_init;
    ret = rt_device_register(&sx1278_device.parent,"sx1278",RT_DEVICE_FLAG_RDONLY);
    #ifdef RT_USING_POSIX
    sx1278_device.parent.fops = &sx1278_fops;
    #endif
    if(ret !=RT_EOK)
    {
        rt_kprintf("sx1278 register failed!\n\r");
        return -RT_ERROR;
    }
    rt_kprintf("sx1278 register success!\n\r");
    return RT_EOK;
}
INIT_APP_EXPORT(sx1278_init);
#endif