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
* @file:    test_i2c.c
* @brief:   a application of i2c function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define I2C_SLAVE_ADDRESS 0x0012U

void TestI2C(void)
{
    // config IIC pin(SCL:34.SDA:35) in menuconfig
    int iic_fd = PrivOpen(I2C_DEV_DRIVER, O_RDWR);
    if (iic_fd < 0)
    {
        printf("open iic_fd fd error:%d\n", iic_fd);
        return;
    }
    printf("IIC open successful!\n");

    // init iic
    uint16 iic_address = I2C_SLAVE_ADDRESS;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = I2C_TYPE;
    ioctl_cfg.args = (void *)&iic_address;

    if (0 != PrivIoctl(iic_fd, OPE_INT, &ioctl_cfg))
    {
        printf("ioctl iic fd error %d\n", iic_fd);
        PrivClose(iic_fd);
        return;
    }
    printf("IIC configure successful!\n");

    // I2C read and write
    char tmp_buff[100];
    while (1)
    {
        PrivTaskDelay(1000);
        PrivWrite(iic_fd, "Hello World!\n", sizeof("Hello World!\n"));
        printf("msg send:%s\n", "Hello World!\n");
        PrivTaskDelay(1000);
        memset(tmp_buff, 0, sizeof(tmp_buff));
        PrivRead(iic_fd, tmp_buff, sizeof(tmp_buff));
        printf("msg recv:%s\n", tmp_buff);
    }

    PrivClose(iic_fd);
    return;
}

PRIV_SHELL_CMD_FUNCTION(TestI2C, a iic test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif