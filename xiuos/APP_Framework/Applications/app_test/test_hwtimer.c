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
* @file:    test_hwtimer.c
* @brief:   a application of hwtimer function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define BSP_LED_PIN 134
#define NULL_PARAMETER 0

static uint16_t pin_fd=0;
static struct PinStat pin_led;

void ledflip(void *parameter)
{
    pin_led.pin = BSP_LED_PIN;
    pin_led.val = !pin_led.val;
    PrivWrite(pin_fd,&pin_led,NULL_PARAMETER);
}

void TestHwTimer(void)
{
    x_ticks_t period = 100000;
    
    pin_fd = PrivOpen(HWTIMER_PIN_DEV_DRIVER, O_RDWR);
    if(pin_fd<0){
        printf("open pin fd error:%d\n",pin_fd);
        return;
    }

    int timer_fd = PrivOpen(HWTIMER_TIMER_DEV_DRIVER, O_RDWR);
    if(timer_fd<0){
        printf("open timer fd error:%d\n",timer_fd);
        return;
    }

    //config led pin in board
     struct PinParam parameter;
     parameter.cmd = GPIO_CONFIG_MODE;
     parameter.pin = BSP_LED_PIN;
     parameter.mode = GPIO_CFG_OUTPUT;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = PIN_TYPE;
    ioctl_cfg.args =  (void *)&parameter;

    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg)) {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }

    ioctl_cfg.ioctl_driver_type = TIME_TYPE;
    ioctl_cfg.args = (void *)&ledflip;
    if (0 != PrivIoctl(timer_fd, OPE_INT, &ioctl_cfg)) {
        printf("timer pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }    

    ioctl_cfg.args = (void *)&period;
    if (0 != PrivIoctl(timer_fd, OPE_CFG, &ioctl_cfg)) {
        printf("timer pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    }    

    while(1){
        
    }

    // int32 timer_handle = KCreateTimer("LED on and off by 1s",&ledflip,&pin_fd,period,TIMER_TRIGGER_PERIODIC);
    
    // KTimerStartRun(timer_handle);
    PrivClose(pin_fd);
    PrivClose(timer_fd);
}

PRIV_SHELL_CMD_FUNCTION(TestHwTimer, a timer test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif