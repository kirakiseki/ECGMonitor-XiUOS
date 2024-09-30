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
* @file:    test_flash.c
* @brief:   a application of flash function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

void TestFlash(void)
{
    int fd = open(FLASH_DEV_DRIVER,O_RDWR);
    if(fd<0){
        printf("fs fd open error:%d\n",fd);
        return;
    }
    struct BusBlockWriteParam flash_writer;
    uint8_t read_buff[8] = {1,2,3,4,5,6,7,8};
    flash_writer.pos = 0x000000L;
    flash_writer.size = 8;
    flash_writer.buffer = read_buff;
    struct BusBlockReadParam flash_reader;
    flash_reader.pos = 0x000000L;
    flash_reader.size = 8;
    flash_reader.buffer = read_buff;    

    PrivRead(fd,&flash_reader,NONE);
    printf("Read data is:");
    for(int i=0;i<flash_writer.size;i++){
        printf("%02x ",read_buff[i]);
        read_buff[i]++;
    }
    printf("\n");

    PrivWrite(fd,&flash_writer,NONE);

    return;
}

PRIV_SHELL_CMD_FUNCTION(TestFlash, a flash test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif