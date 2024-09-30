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
* @file:    test_rs485.c
* @brief:   a application of rs485 function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define BSP_485_DIR_PIN 24

void Test485(void)
{
    int pin_fd = PrivOpen(RS485_PIN_DEV_DRIVER, O_RDWR);
    if (pin_fd < 0)
    {
        printf("open pin fd error:%d\n", pin_fd);
        return;
    }

    int uart_fd = PrivOpen(RS485_UART_DEV_DRIVER, O_RDWR);
    if (uart_fd < 0)
    {
        printf("open pin fd error:%d\n", uart_fd);
        return;
    }
    printf("uart and pin fopen success\n");

    //config led pin in board
    struct PinParam pin_parameter;
    memset(&pin_parameter, 0, sizeof(struct PinParam));
    pin_parameter.cmd = GPIO_CONFIG_MODE;
    pin_parameter.pin = BSP_485_DIR_PIN;
    pin_parameter.mode = GPIO_CFG_OUTPUT;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = PIN_TYPE;
    ioctl_cfg.args =  (void *)&pin_parameter;

    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg)) {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }

    struct SerialDataCfg uart_cfg;
    memset(&uart_cfg, 0, sizeof(struct SerialDataCfg));

    uart_cfg.serial_baud_rate = BAUD_RATE_115200;
    uart_cfg.serial_data_bits = DATA_BITS_8;
    uart_cfg.serial_stop_bits = STOP_BITS_1;
    uart_cfg.serial_parity_mode = PARITY_NONE;
    uart_cfg.serial_bit_order = BIT_ORDER_LSB;
    uart_cfg.serial_invert_mode = NRZ_NORMAL;
    uart_cfg.serial_buffer_size = SERIAL_RB_BUFSZ;
    uart_cfg.serial_timeout = 1000;
    uart_cfg.is_ext_uart = 0;

    ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
    ioctl_cfg.args = (void *)&uart_cfg;

    if (0 != PrivIoctl(uart_fd, OPE_INT, &ioctl_cfg))
    {
        printf("ioctl uart fd error %d\n", uart_fd);
        PrivClose(uart_fd);
        return;
    }

    struct PinStat pin_dir;
    pin_dir.pin = BSP_485_DIR_PIN;
    while (1)
    {
        pin_dir.val = GPIO_HIGH;
        PrivWrite(pin_fd,&pin_dir,0);
        PrivWrite(uart_fd,"Hello world!\n",sizeof("Hello world!\n"));
        PrivTaskDelay(100);

        pin_dir.val = GPIO_LOW;
        PrivWrite(pin_fd,&pin_dir,0);
        char recv_buff[100];
        memset(recv_buff,0,sizeof(recv_buff));
        PrivRead(uart_fd,recv_buff,20);
        printf("%s",recv_buff);
        PrivTaskDelay(100);
    }
    PrivClose(pin_fd);
    PrivClose(uart_fd);
    return;
}

PRIV_SHELL_CMD_FUNCTION(Test485, a RS485 test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif