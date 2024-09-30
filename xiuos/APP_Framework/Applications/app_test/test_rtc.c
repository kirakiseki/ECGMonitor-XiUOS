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
* @file:    test_rtc.c
* @brief:   a application of rtc function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

void TestRTC(int argc,char *argv[])
{
    int rtc_fd = PrivOpen(RTC_DEV_DRIVER, O_RDWR);
    if(rtc_fd<0){
        printf("open rtc fd error:%d\n",rtc_fd);
        return;
    }
    
    if(argc>1){
        int times = atoi(argv[1]);
        printf("Time will be printf %d times\n",times);

        struct RtcDrvConfigureParam rtc_para;
        time_t my_time=0;

        rtc_para.rtc_operation_cmd = OPER_RTC_SET_TIME;
        rtc_para.time = &my_time;

        struct PrivIoctlCfg ioctl_cfg;
        ioctl_cfg.ioctl_driver_type = RTC_TYPE;
        ioctl_cfg.args =  (void *)&rtc_para;

        PrivIoctl(rtc_fd,0,&ioctl_cfg);

        rtc_para.rtc_operation_cmd = OPER_RTC_GET_TIME;
        for(size_t i=0;i<times;i++){
            PrivIoctl(rtc_fd,0,&ioctl_cfg);
            printf("The time now is %d\n",*(rtc_para.time));
            PrivTaskDelay(5000);  
        }
    }
    
    PrivClose(rtc_fd);
    return;
}

PRIV_SHELL_CMD_FUNCTION(TestRTC, a rtc test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif