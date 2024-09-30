#ifndef __DRV_SX1278_H__
#define __DRV_SX1278_H__

#include <rtthread.h>
#include <rtdevice.h>

#include <rthw.h>
#define SPI_NAME "spi20"
#define LORA_BUFFER_SIZE 256
struct BlockReadParam
{
    rt_off_t pos;
    void * buffer;
    rt_size_t size;
};
struct BlockWriteParam
{
    rt_off_t pos;
    const void *buffer;
    rt_size_t size;
};
#endif



