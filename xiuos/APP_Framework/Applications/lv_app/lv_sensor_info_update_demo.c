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
* @file:    lv_sensor_info_update_demo.c
* @brief:   a sensor info update application using littleVgl
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#include "lv_sensor_info.h"

void* lvgl_thd_sensor_info_update_demo(void *parameter)
{
    double val = 0;
    while (1)
    {
        sensor_update_val(val + 0.1, O3);
        sensor_update_val(val, CO2);
        sensor_update_val(val + 0.2, NO2);
        sensor_update_val(val - 0.1, SO2);
        sensor_update_val(val + 0.3, AQS);
        sensor_update_val(val - 0.3, O2);
        sensor_update_val(val + 0.3, TEMPERATURE);
        val += 0.3;
        PrivTaskDelay(10);
    }
}

pthread_t lvgl_task;
static int lvgl_sensor_info_update_demo(void)
{
    pthread_attr_t attr;
    attr.schedparam.sched_priority = 25;
    attr.stacksize = 4096;

    PrivTaskCreate(&lvgl_task, &attr, lvgl_thd_sensor_info_update_demo, NULL);

    return 0;
}
PRIV_SHELL_CMD_FUNCTION(lvgl_sensor_info_update_demo, a littlevgl sensor infor update sample, PRIV_SHELL_CMD_MAIN_ATTR);
