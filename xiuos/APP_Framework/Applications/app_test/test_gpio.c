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
* @file:    test_gpio.c
* @brief:   a application of GPIO function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#ifdef BOARD_EDU_RISCV64_EVB
#define BSP_LED_PIN 29
#define BSP_KEY_PIN 31
#elif defined BOARD_EDU_ARM32_EVB
#define BSP_LED_PIN 134
#define BSP_KEY_PIN 176
#endif
#define NULL_PARAMETER 0

void  TestGpio(void)
{
    int pin_fd = PrivOpen(GPIO_DEV_DRIVER, O_RDWR);
    if(pin_fd < 0) {
        printf("open pin fd error:%d\n",pin_fd);
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

    //config key pin in board
    parameter.pin = BSP_KEY_PIN;   
    parameter.mode = GPIO_CFG_INPUT;

    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg)) {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return;
    } 

    struct PinStat pin_led;
    struct PinStat pin_key;

    pin_led.pin = BSP_LED_PIN;
    pin_key.pin = BSP_KEY_PIN;
    
    //recycle read pin and write pin until key break
    while(1){
        if(0 > PrivRead(pin_fd, &pin_key, NULL_PARAMETER)) {
            printf("read pin fd error %d\n", pin_fd);
            PrivClose(pin_fd);
            return;
        }

        //led on if key pressed,or led off
        if(pin_key.val){
            pin_led.val = GPIO_HIGH;
        }else{
            pin_led.val = GPIO_LOW;
        }            
        
        if(0 > PrivWrite(pin_fd, &pin_led, NULL_PARAMETER)) {
            printf("write pin fd error %d\n", pin_fd);
            PrivClose(pin_fd);
            return;
        }           
    }
}

PRIV_SHELL_CMD_FUNCTION(TestGpio, a gpio test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif