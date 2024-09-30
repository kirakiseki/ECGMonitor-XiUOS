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
 * @file modbus_rtu.c
 * @brief Implement the connection Modbus RTU adapter function, using Modbus rtu device
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2021.07.08
 */

#include <adapter.h>
#define PLC_DATA_LENGTH 1024
struct CircularAreaApp *g_circular_area;
static pthread_t recv_plc_data_task;
/**
 * @description: Open modbus rtu function
 * @param adapter - Modbus RTU device pointer
 * @return success: 0, failure: -1
 */
static int ModbusRtuOpen(struct Adapter *adapter)
{
     
    /* open  serial port*/
    adapter->fd = PrivOpen("/dev/tty3", 2);//2 read and write
    if (adapter->fd < 0) {
        printf("ModbusRtuOpen get serial %s fd error\n", "/dev/tty3");
        return -1;
    }

    PrivTaskDelay(2500);
    
    ADAPTER_DEBUG("Modbus rtu open done\n");

    return 0;
}

/**
 * @description: Close modbus rtu function
 * @param adapter - Modbus RTu device pointer
 * @return success: 0, failure: -1
 */
static int ModbusRtuClose(struct Adapter *adapter)
{
    /*step1: close modbus serial port*/
    int ret;
    ret = PrivClose(adapter->fd);
    if(ret < 0){
        printf("ModbusRtuOpen close serial %s fd error\n", "/dev/tty3");
        return -1;
    }

    ADAPTER_DEBUG("Modbus rtu Close done\n");

    return 0;
}

static int ModbusRtuIoctl(struct Adapter *adapter, int cmd)
{
    
   // uint32_t baud_rate = *((uint32_t *)args);

    struct SerialDataCfg serial_cfg;
    memset(&serial_cfg, 0 ,sizeof(struct SerialDataCfg));
    serial_cfg.serial_baud_rate = BAUD_RATE_9600 ;
    serial_cfg.serial_data_bits = DATA_BITS_8;
    serial_cfg.serial_stop_bits = STOP_BITS_1;
    serial_cfg.serial_buffer_size = SERIAL_RB_BUFSZ;
    serial_cfg.serial_parity_mode = PARITY_EVEN;
    serial_cfg.serial_bit_order = STOP_BITS_1;
    serial_cfg.serial_invert_mode = NRZ_NORMAL;
    serial_cfg.is_ext_uart = 0;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
    ioctl_cfg.args = &serial_cfg;
    PrivIoctl(adapter->fd, OPE_INT, &ioctl_cfg);
       
    return 0;
}

static int ModbusRtuSend(struct Adapter *adapter, const void *buf, size_t len)
{
    int ret;
    ret = PrivWrite(adapter->fd, buf, len);
    if(ret < 0){
        printf("send failed %d!\n", ret);
    }
    
    return ret;
}

static int ModbusRtuRecv(struct Adapter *adapter, void *buf, size_t len)
{
     int recv_len=0, recv_len_continue=0;

    uint8 *recv_buf = PrivMalloc(len);

    recv_len = PrivRead(adapter->fd, recv_buf, len);
    if (recv_len) {
        while (recv_len < len) {
            recv_len_continue = PrivRead(adapter->fd, recv_buf + recv_len, len - recv_len);
            if (recv_len_continue) {
                recv_len += recv_len_continue;
            } else {
                recv_len = 0;
                break;
            }
        }
        memcpy(buf, recv_buf, len);
    }

    PrivFree(recv_buf);
    
    return recv_len;
}


static const struct IpProtocolDone ec200t_done = 
{
    .open = ModbusRtuOpen,
    .close = ModbusRtuClose,
    .ioctl = ModbusRtuIoctl,
    .setup = NULL,
    .setdown = NULL,
    .setaddr = NULL,
    .setdns = NULL,
    .setdhcp = NULL,
    .ping = NULL,
    .netstat = NULL,
    .connect = NULL,
    .send = ModbusRtuSend,
    .recv = ModbusRtuRecv,
    .disconnect = NULL,
};