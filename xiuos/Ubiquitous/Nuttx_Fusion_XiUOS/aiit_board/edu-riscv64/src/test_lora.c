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
 * @file test_lora.c
 * @brief edu-riscv64 test_lora.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.12.13
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdio.h>
#include <nuttx/fs/fs.h>
#include <nuttx/fs/ioctl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <nuttx/time.h>
#include <nuttx/ioexpander/gpio.h>

void TestLora(void)
{
    int fd, m0fd, m1fd;
    int i;
    char sendbuffer1[4] = {0xC0,0x04,0x01,0x09};
    char sendbuffer2[6] = {0xC0,0x00,0x03,0x12,0x34,0x61};
    char sendbuffer3[3] = {0xC1,0x04,0x01};
    char sendbuffer4[3] = {0xC1,0x00,0x03};
    char buffer[256];
    int readlen;
    struct termios term;

    fd = open("/dev/ttyS2", O_RDWR);
    tcgetattr(fd, &term);
    cfsetspeed(&term, 9600);
    tcsetattr(fd, TCSANOW, &term);

    m0fd = open("/dev/gpio0", O_RDWR);
    m1fd = open("/dev/gpio1", O_RDWR);
#ifdef CONFIG_ADAPTER_E22
    ioctl(m0fd, GPIOC_WRITE, (unsigned long)0);
#else
    ioctl(m0fd, GPIOC_WRITE, (unsigned long)1);
#endif
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
