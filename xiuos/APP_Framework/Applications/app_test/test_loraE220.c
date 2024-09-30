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
* @file:    test_loraE220.c
* @brief:   a application of loraE220 function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define NULL_PARAMETER 0
#define E220_CFG_LENGTH
#define GPIOSET(fd, buf, bit)                        \
    {                                                \
        buf.val = bit;                               \
        if (0 > PrivWrite(fd, &buf, NULL_PARAMETER)) \
        {                                            \
            printf("write pin fd error %d\n", fd);   \
            PrivClose(fd);                           \
            return;                                  \
        }                                            \
    }
#define BSP_E220_M0_PIN 32
#define BSP_E220_M1_PIN 33

void TestLora(int argc, char *argv[])
{
    char uart_recvbuff[100];
    memset(uart_recvbuff, 0, sizeof(uart_recvbuff));

    int pin_fd = PrivOpen(LORA_PIN_DEV_DRIVER, O_RDWR);
    if (pin_fd < 0)
    {
        printf("open pin fd error:%d\n", pin_fd);
        return;
    }

    int uart_fd = PrivOpen(LORA_UART_DEV_DRIVER, O_RDWR);
    if (uart_fd < 0)
    {
        printf("open pin fd error:%d\n", uart_fd);
        return;
    }
    printf("uart and pin fopen success\n");

    struct PinStat pin_m0;
    struct PinStat pin_m1;
    pin_m0.pin = BSP_E220_M0_PIN;
    pin_m1.pin = BSP_E220_M1_PIN;

    // config led pin in board
    struct PrivIoctlCfg ioctl_cfg;
    struct PinParam pin_param;
    pin_param.cmd = GPIO_CONFIG_MODE;
    pin_param.mode = GPIO_CFG_OUTPUT;
    pin_param.pin = BSP_E220_M0_PIN;
    ioctl_cfg.ioctl_driver_type = PIN_TYPE;
    ioctl_cfg.args = &pin_param;
    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg))
    {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }

    pin_param.pin = BSP_E220_M1_PIN;
    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg))
    {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }

    printf("pin configure success\n");
    struct SerialDataCfg uart_cfg;

    // loraE220 support only 9600bps with 8N1 during initializing
    uart_cfg.serial_baud_rate = BAUD_RATE_9600;
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
    printf("uart configure success\n");

    GPIOSET(pin_fd, pin_m0, GPIO_HIGH);
    GPIOSET(pin_fd, pin_m1, GPIO_HIGH);
    printf("lora configure into sleep(configure) mode\n");

    // send configure data, and receive the same length of data
    // configure loraE220 as address 1949 CH17 36.8kps
    char sendbuff[] = {0xC0, 0x00, 0x05, 0x19, 0x49, 0xE6, 0x00, 0x17}; 

    PrivTaskDelay(2000);

    printf("Sending lora configure information(SIZE:%d)\n", sizeof(sendbuff));
    PrivWrite(uart_fd, sendbuff, sizeof(sendbuff));
    printf("lora configure information send\n");

    PrivTaskDelay(2000);

    PrivRead(uart_fd, uart_recvbuff, sizeof(sendbuff));
    printf("%x %x %x %x", uart_recvbuff[0], uart_recvbuff[1], uart_recvbuff[2], uart_recvbuff[3]);
    printf("lora configure success\n");

    // error when all bytes are 0xff
    if (0xFF == (uart_recvbuff[0] & uart_recvbuff[1] & uart_recvbuff[2]))
    {
        printf("from lora receive error:%d\n", 0xff);
        return;
    }

    uart_cfg.serial_baud_rate = BAUD_RATE_115200;
    if (0 != PrivIoctl(uart_fd, OPE_INT, &ioctl_cfg))
    {
        printf("ioctl uart fd error %d\n", uart_fd);
        PrivClose(uart_fd);
        return;
    }

    // into transparent transmission mode
    GPIOSET(pin_fd, pin_m0, GPIO_LOW);
    GPIOSET(pin_fd, pin_m1, GPIO_LOW);
    // receive and send "Hello World"
    while (1)
    {
        PrivTaskDelay(500);
        PrivWrite(uart_fd, "Hello_World!", sizeof("Hello_World!"));
        printf("Data Send:\n%s\n", "Hello_World!");

        PrivTaskDelay(500);
        memset(uart_recvbuff, 0, sizeof(uart_recvbuff));
        PrivRead(uart_fd, uart_recvbuff, sizeof(uart_recvbuff));
        printf("Receive Data is :\n%s\n", uart_recvbuff);
    }
    PrivClose(pin_fd);
    PrivClose(uart_fd);
}

PRIV_SHELL_CMD_FUNCTION(TestLora, a lora test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif