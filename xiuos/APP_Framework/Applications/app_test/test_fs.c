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
* @file:    test_fs.c
* @brief:   a application of file_system function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/11/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define MAX_READ_LENGTH 1000

// sd card here is loaded as "/"
void TestFs(void)
{
    //open the file in sdcard
    int fd = open(FPATH,O_RDWR|O_CREAT);
    if(fd<0){
        printf("fs fd open error:%d\n",fd);
        return;
    }

    char filewords[MAX_READ_LENGTH];
    memset(filewords,0,MAX_READ_LENGTH);
    const char *input_words = "these words are going to write in fs\n";

    //read and write then close file
    int err_flag = read(fd,filewords,MAX_READ_LENGTH);
    if(err_flag<0){
        printf("read failed,error:%d\n",err_flag);
        return;
    }
    printf("read data is \n%s\n",filewords);
    
    err_flag = write(fd,input_words,strlen(input_words));
    if(err_flag<0){
        printf("write failed,error:%d\n",err_flag);
        return;
    }
    err_flag = close(fd);
    if(err_flag<0){
        printf("close failed,error %d\n",err_flag);
        return ;
    }

    //re-open the file and re-read the file
    fd = open(FPATH,O_RDWR);
     if(fd<0){
        printf("fs fd open error:%d\n",fd);
        return;
    }   
    err_flag = read(fd,filewords,MAX_READ_LENGTH);
    if(err_flag<0){
        printf("read failed,error:%d\n",err_flag);
        return;
    }    

    printf("read data is \n%s\n",filewords);
    err_flag = close(fd);
    if(err_flag<0){
        printf("close failed,error:%d\n",err_flag);
        return;
    }        

    return;
}

PRIV_SHELL_CMD_FUNCTION(TestFs, a sd or usb filesystem test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif