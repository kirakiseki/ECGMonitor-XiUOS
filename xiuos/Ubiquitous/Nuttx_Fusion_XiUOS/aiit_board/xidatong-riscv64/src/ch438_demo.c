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
 * @file ch438_demo.c
 * @brief xidatong-riscv64 ch438_demo.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "k210_ch438.h"
#include <sys/ioctl.h>
#include <nuttx/ioexpander/gpio.h>

void CH438Demo(void)
{
    int fd, m0fd, m1fd;
    int i;
    char sendbuffer1[4] = {0xC0,0x04,0x01,0x09};
    char sendbuffer2[6] = {0xC0,0x00,0x03,0x12,0x34,0x61};
    char sendbuffer3[3] = {0xC1,0x04,0x01};
    char sendbuffer4[3] = {0xC1,0x00,0x03};
    char buffer[256];
    int readlen;

    fd = open("/dev/extuart_dev3", O_RDWR);
    ioctl(fd, OPE_INT, (unsigned long)9600);
    m0fd = open("/dev/gpio0", O_RDWR);
    m1fd = open("/dev/gpio1", O_RDWR);
    ioctl(m0fd, GPIOC_WRITE, (unsigned long)1);
    ioctl(m1fd, GPIOC_WRITE, (unsigned long)1);
    sleep(1);

    write(fd, sendbuffer1,4);
    sleep(1);
    readlen = read(fd, buffer, 256);
    printf("readlen1 = %d\n", readlen);
    for(i = 0;i< readlen; ++i)
    {
        printf("0x%x\n", buffer[i]);
    }

    write(fd, sendbuffer2,6);
    sleep(1);
    readlen = read(fd, buffer, 256);
    printf("readlen1 = %d\n", readlen);
    for(i = 0;i< readlen; ++i)
    {
        printf("0x%x\n", buffer[i]);
    }

    write(fd, sendbuffer3,3);
    sleep(1);
    readlen = read(fd, buffer, 256);
    printf("readlen1 = %d\n", readlen);
    for(i = 0;i< readlen; ++i)
    {
        printf("0x%x\n", buffer[i]);
    }

    write(fd, sendbuffer4,3);
    sleep(1);
    readlen = read(fd, buffer, 256);
    printf("readlen1 = %d\n", readlen);
    for(i = 0;i< readlen; ++i)
    {
        printf("0x%x\n", buffer[i]);
    }

    close(fd);
}
