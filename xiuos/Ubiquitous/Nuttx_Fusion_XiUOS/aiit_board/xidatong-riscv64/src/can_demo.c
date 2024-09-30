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
 * @file can_demo.c
 * @brief xidatong-riscv64 can_demo.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.11.10
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <nuttx/pthread.h>
#include <sys/ioctl.h>
#include <nuttx/time.h>
#include <nuttx/fs/fs.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <arch/board/board.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "time.h"
#include <debug.h>
#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <nuttx/ioexpander/gpio.h>
#include "k210_uart.h"
#include "k210_fpioa.h"
#include "k210_gpiohs.h"
#include "k210_gpio_common.h"

static int fd, flag=0;

static void serial_thread_entry(void)
{
    uint8_t ch;
    while(read(fd, &ch, 1) == 1)
    {
        printf("%02x ",ch);
    }
}

static void start_thread(void)
{
    int ret;
    pthread_t thread;   
    pthread_attr_t attr = PTHREAD_ATTR_INITIALIZER;
    attr.priority = 20;
    attr.stacksize = 2048;

    ret = pthread_create(&thread, &attr, (void*)serial_thread_entry, NULL);
    if (ret != 0)
    {
        printf("task create failed, status=%d\n", ret);
    }

    flag = 1;
}

static void set_baud(unsigned long speed)
{
    struct termios cfg;
    tcgetattr(fd, &cfg);
    cfsetspeed(&cfg, speed);
    tcsetattr(fd, TCSANOW, &cfg);
}

static void can_cfg_start(void)
{ 
    uint8_t cmd[8];
    set_baud(9600);
    up_mdelay(1000);

    k210_gpiohs_set_direction(FPIOA_CAN_NCFG, GPIO_DM_OUTPUT);
    k210_gpiohs_set_value(FPIOA_CAN_NCFG, GPIO_PV_LOW);
    up_mdelay(200);

    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = 0xFD; 
    cmd[3] = 0x32;
    cmd[4] = 0x01;
    cmd[5] = 0x0B;
    cmd[6] = 0xc4;
    cmd[7] = 0x29;
    write(fd, cmd, 8);
}

static void can_cfg_end(void)
{
    k210_gpiohs_set_direction(FPIOA_CAN_NCFG, GPIO_DM_OUTPUT);
    k210_gpiohs_set_value(FPIOA_CAN_NCFG, GPIO_PV_HIGH);
    set_baud(115200);
}

void can_test(void)
{
	uint8_t msg[8];
	uint8_t i;
	
	fd = open("/dev/ttyS1", O_RDWR);
    if (flag == 0) 
    {   
        /* 1、start thread */
        start_thread();
        up_mdelay(20);

        /* 2、config can prama */
        can_cfg_start();
        up_mdelay(20);

        /* 3、exit config */
        can_cfg_end();
        up_mdelay(20);;
    }

    /* 4、send data */
    for(i=0;i<10;i++)
    {
        msg[0] = 0x11;
        msg[1] = 0x22;
        msg[2] = 0x33;
        msg[3] = 0x44;
        msg[4] = 0x55;
        msg[5] = 0x66;
        msg[6] = 0x77;
        msg[7] = 0x99;
        write(fd, msg, 8);
        up_mdelay(20);
    }
}