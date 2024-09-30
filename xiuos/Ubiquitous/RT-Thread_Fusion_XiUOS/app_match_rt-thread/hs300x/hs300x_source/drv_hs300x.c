#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <drv_hs300x.h>
#include <dfs_file.h>
#ifdef RT_USING_POSIX
#include <dfs_posix.h>
#include <dfs_poll.h>
#endif
#ifdef DRV_USING_HS300X
static rt_bool_t init_flag = RT_FALSE;
struct temp_humi_device
{
    struct rt_device parent;
};
struct temp_humi_device hs300x_device;
static struct rt_i2c_bus_device *i2c_bus = RT_NULL;


static rt_err_t rt_hs300x_open(rt_device_t dev,rt_uint16_t oflag)
{
    RT_ASSERT(dev !=RT_NULL);
    return RT_EOK;
}

static rt_err_t rt_hs300x_close(rt_device_t dev)
{
    RT_ASSERT(dev !=RT_NULL);
    return RT_EOK;
}

static rt_err_t hs300x_i2c_write(struct rt_i2c_bus_device *bus,rt_uint8_t len,rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = HS300X_DEV_ADDRESS;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = len;

    if(rt_i2c_transfer(bus,&msgs,1)==1)
    {

        return RT_EOK;
    }
    else
    {

        return -RT_ERROR;
    }

}
static rt_err_t hs300x_i2c_read(struct rt_i2c_bus_device *bus,rt_uint8_t len,rt_uint8_t *buf)
{

    struct rt_i2c_msg msg[2];
    msg[0].addr = HS300X_DEV_ADDRESS;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf = RT_NULL;
    msg[0].len = 0;

    msg[1].addr = HS300X_DEV_ADDRESS;
    msg[1].flags = RT_I2C_RD;
    msg[1].buf = buf;
    msg[1].len = len;

    if(rt_i2c_transfer(bus,&msg,2)==2)
    {
        return RT_EOK;

    }
    else{
        return -RT_ERROR;
    }
}

static rt_size_t rt_hs300x_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
    RT_ASSERT(dev!=RT_NULL);
    return RT_EOK;

}

static rt_size_t rt_hs300x_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    RT_ASSERT(dev !=RT_NULL);
    return RT_EOK;
}

static rt_err_t  rt_hs300x_init  (rt_device_t dev)
{
    RT_ASSERT(dev!=RT_NULL);
    return RT_EOK;
}

static void read_temp_humi(float *cur_temp,float *cur_humi)
{
    rt_uint8_t temperature[4];

    hs300x_i2c_read(i2c_bus,4,temperature);
    *cur_temp = ((temperature[2]<<8|temperature[3])>>2)*165.0 /((1<<14)-1)-40;
    *cur_humi = ((temperature[0]<<8|temperature[1])& 0x3fff)*100.0 /((1<<14)-1);
}

static void hs300x_i2c_sample(void)
{
    float hs300x_temp = 0.0;
    float hs300x_humi = 0.0;
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_NAME);
    if(i2c_bus ==RT_NULL)
    {
        rt_kprintf("device find err\n");
        return -RT_ERROR;
    }
    for(int i=0;i<100;i++)
    {
        read_temp_humi(&hs300x_temp,&hs300x_humi);
        rt_kprintf("humidity: %d.%d %%\n",(int)hs300x_humi,(int)(hs300x_humi*10)%10);
        if(hs300x_temp>0)
        {
            rt_kprintf("temperature:%d.%d C\n",(int)hs300x_temp,(int)(hs300x_temp*10)%10);
        }
        else
        {
            rt_kprintf("temperature:%d.%d C\n",(int)hs300x_temp,(int)(-hs300x_temp*10)%10);
        }
        rt_thread_mdelay(5000);
    }
}

static int hs300x_fops_open(struct dfs_fd *fd)
{
    rt_err_t ret;
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev !=RT_NULL);
    ret = rt_device_open(dev,RT_DEVICE_FLAG_RDONLY);
    if(ret == RT_EOK)
    {
        return RT_EOK;
    }
    return RT_ERROR;
}
static int hs300x_fops_close(struct dfs_fd *fd)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    rt_device_set_rx_indicate(dev,RT_NULL);
    rt_device_close(dev);
    return RT_EOK;
}
static int hs300x_fops_read(struct dfs_fd *fd,void *buf,size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev!=RT_NULL);
    rt_err_t ret;
    if(count !=4 )
    {
        rt_kprintf("can only read 4bytes!\n");
    }
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_NAME); 
    ret = hs300x_i2c_read(i2c_bus,count,buf);
    if (ret ==RT_EOK)
    {
        return RT_EOK;
    }
    return RT_ERROR;
}
static int hs300x_fops_write(struct dfs_fd *fd,const void *buf,size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t)fd->data;
    RT_ASSERT(dev !=RT_NULL);
    rt_err_t ret;
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_NAME);
    ret = hs300x_i2c_write(i2c_bus,0,RT_NULL);
    if(ret ==RT_EOK)
    {
        return RT_EOK;
    }
    return RT_ERROR;
}

const static struct dfs_file_ops hs300x_fops = 
{
    hs300x_fops_open,
    hs300x_fops_close,
    RT_NULL,
    hs300x_fops_read,
    hs300x_fops_write,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL, 
};



int sensor_hs300x_init()
{
    rt_err_t ret = RT_EOK;
    hs300x_device.parent.open = rt_hs300x_open;
    hs300x_device.parent.close = rt_hs300x_close;
    hs300x_device.parent.read = rt_hs300x_read;
    hs300x_device.parent.write = rt_hs300x_write;
    hs300x_device.parent.init = rt_hs300x_init;
    ret = rt_device_register(&hs300x_device.parent,"hs300x",RT_DEVICE_FLAG_RDONLY);
    #ifdef RT_USING_POSIX
    hs300x_device.parent.fops = &hs300x_fops;
    #endif
    if(ret !=RT_EOK)
    {
        rt_kprintf("hs300x register failed!\n\r");
        return -RT_ERROR;
    }
    rt_kprintf("hs300x register success!\n\r");
    return RT_EOK;
}
INIT_APP_EXPORT(sensor_hs300x_init);
MSH_CMD_EXPORT(hs300x_i2c_sample, a read temp sample);

#endif