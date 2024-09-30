/*
* Copyright (c) 2022 AIIT XUOS Lab
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
 * @file control_io.c
 * @brief low level io code for control framework 
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-11-21
 */

#include <control_io.h>

#ifdef CONTROL_USING_SERIAL_485
static int pin_fd = 0;
static int uart_fd = 0;

/**
 * @description: Set Uart 485 Input
 * @return 
 */
static void Set485Input(void)
{
    struct PinStat pin_stat;
    pin_stat.pin = CONTROL_FRAMEWORK_UART_485_DIR;
    pin_stat.val = GPIO_LOW;
    PrivWrite(pin_fd, &pin_stat, 1);
}

/**
 * @description: Set Uart 485 Output
 * @return
 */
static void Set485Output(void)
{
    struct PinStat pin_stat;
    pin_stat.pin = CONTROL_FRAMEWORK_UART_485_DIR;
    pin_stat.val = GPIO_HIGH;
    PrivWrite(pin_fd, &pin_stat, 1);
}

/**
 * @description: Control Framework Uart 485 Init
 * @param baud_rate - baud rate
 * @param data_bits - data bits
 * @param stop_bits - stop bits
 * @param check_mode - check mode, even、odd、none
 * @return
 */
void Uart485Init(uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t check_mode)
{
    int ret = 0;

    pin_fd = PrivOpen(CONTROL_FRAMEWORK_PIN_DEV, O_RDWR);
    if (pin_fd < 0) {
        printf("open %s error\n", CONTROL_FRAMEWORK_PIN_DEV);
        return;
    }

    struct PinParam pin_param;
    pin_param.cmd = GPIO_CONFIG_MODE;
    pin_param.mode = GPIO_CFG_OUTPUT;
    pin_param.pin = CONTROL_FRAMEWORK_UART_485_DIR;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = PIN_TYPE;
    ioctl_cfg.args = &pin_param;
    PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg);

    uart_fd = open(CONTROL_FRAMEWORK_UART_DEV, O_RDWR);
    if (uart_fd < 0) {
        printf("open fd error %d\n", uart_fd);
        return;
    }
    printf("Uart485Init open fd %d baud_rate %d data_bits %d stop_bits %d check_mode %d\n", 
        uart_fd, baud_rate, data_bits, stop_bits, check_mode);

    struct SerialDataCfg cfg;
    cfg.serial_baud_rate = baud_rate;
    cfg.serial_data_bits = data_bits;
    cfg.serial_stop_bits = stop_bits;
    cfg.serial_buffer_size = 128;
    cfg.serial_parity_mode = check_mode;
    cfg.serial_bit_order = 0;
    cfg.serial_invert_mode = 0;
#ifdef CONTROL_FRAMEWORK_DRIVER_EXTUART
    cfg.ext_uart_no = 0;
    cfg.port_configure = PORT_CFG_INIT;
#endif
    cfg.serial_timeout = 10000;

    ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
    ioctl_cfg.args = &cfg;
    ret = PrivIoctl(uart_fd, OPE_INT, &ioctl_cfg);
    if (0 != ret) {
        printf("ioctl fd error %d\n", ret);
        return;
    }

    printf("%s done!\n", __func__);
}
#endif

/**
 * @description: Control Framework Socket Init
 * @param ip - local ip pointer
 * @param mask - netmask pointer
 * @param gw - gateway pointer
 * @return
 */
void SocketInit(char *ip, char *mask, char *gw)
{
    printf("%s ip %d.%d.%d.%d mask %d.%d.%d.%d gw %d.%d.%d.%d\n", __func__, 
        ip[0], ip[1], ip[2], ip[3],
        mask[0], mask[1], mask[2], mask[3],
        gw[0], gw[1], gw[2], gw[3]);
#ifdef CONTROL_USING_SOCKET
#ifdef BSP_USING_LWIP
    lwip_config_tcp(0, ip, mask, gw);
#endif
#ifdef BSP_USING_W5500
    //to do
#endif
#endif
}

/**
 * @description: Control Framework Serial Init
 * @param baud_rate - baud rate
 * @param data_bits - data bits
 * @param stop_bits - stop bits
 * @param check_mode - check mode
 * @return
 */
void SerialInit(uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t check_mode)
{
#ifdef CONTROL_USING_SERIAL_485
    Uart485Init(baud_rate, data_bits, stop_bits, check_mode);
#endif
}

/**
 * @description: Control Framework Serial Write
 * @param write_data - write data
 * @param length - length
 * @return
 */
void SerialWrite(uint8_t *write_data, int length)
{
#ifdef CONTROL_USING_SERIAL_485
    Set485Output();
    PrivTaskDelay(20);

    PrivWrite(uart_fd, write_data, length);

    PrivTaskDelay(15);
    Set485Input();
#endif
}

/**
 * @description: Control Framework Serial Read
 * @param read_data - read data
 * @param length - length
 * @return read data size
 */
int SerialRead(uint8_t *read_data, int length)
{
#ifdef CONTROL_USING_SERIAL_485
    int data_size = 0;
    int data_recv_size = 0;

    while (data_size < length) {
        data_recv_size = PrivRead(uart_fd, read_data + data_recv_size, length);
        data_size += data_recv_size;
    }

    //need to wait 30ms , make sure write cmd again and receive data successfully
    PrivTaskDelay(30);

    return data_size;
#endif
}
