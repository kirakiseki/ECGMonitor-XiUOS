/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file e22_main.c
 * @brief e22 demo
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.05.20
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <nuttx/ioexpander/gpio.h>

int main(int argc, FAR char *argv[])
{
    int fd, m0fd, m1fd;
    int ret;
    int i;
    struct termios tio;

    char sendbuffer1[4] = {0xC0,0x05,0x01,0x09};
    char sendbuffer2[7] = {0xC0,0x00,0x04,0x12,0x34,0x00,0x61};
    char sendbuffer3[3] = {0xC1,0x00,0x04};
    char buffer[256];
    int readlen;

    fd = open("/dev/ttyS3", O_RDWR);
    if(fd < 0)
    {
      printf("Error opening serial: %d\n", fd);;
    }
    
    ret = tcgetattr(fd, &tio);
    if (ret < 0)
    {
        printf("Error getting attributes: %d\n", ret);
    }
    
    ret = cfsetspeed(&tio, B9600);
    if (ret < 0)
    {
        printf("Error setting baud rate: %d\n", ret);
    }
    
    ret = tcsetattr(fd, TCSANOW, &tio);
    if (ret < 0)
    {
        printf("Error getting attributes: %d\n", ret);
    }
    

    m0fd = open("/dev/gpout0", O_RDWR);
    m1fd = open("/dev/gpout1", O_RDWR);
    ioctl(m0fd, GPIOC_WRITE, (unsigned long)0);
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

    write(fd, sendbuffer2,7);
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

    close(fd);
    close(m0fd);
    close(m1fd);
    
    return 0;  
}
