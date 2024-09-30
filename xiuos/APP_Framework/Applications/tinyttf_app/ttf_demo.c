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
* @file:    ttf_demo.c
* @brief:   a application using tinyttf
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2023/3/15
*
*/

#include <lvgl.h>
#include <lv_port_indev_template.h>
#include <transform.h>

extern void lv_example_tiny_ttf_1(void);
extern void lv_example_tiny_ttf_2(void);
void* ttf_thread(void *parameter)
{
    /* display demo; you may replace with your LVGL application at here */
    lv_example_tiny_ttf_1();
    // lv_example_tiny_ttf_1();

    /* handle the tasks of LVGL */
    while(1)
    {
        lv_task_handler();
        PrivTaskDelay(10);
    }
}

pthread_t lvgl_task;
static int ttf_demo_init(void)
{
    pthread_attr_t attr;
    attr.schedparam.sched_priority = 25;
    attr.stacksize = 4096;

    PrivTaskCreate(&lvgl_task, &attr, ttf_thread, NULL);

    return 0;
}
PRIV_SHELL_CMD_FUNCTION(ttf_demo_init, a tinyttf init sample, PRIV_SHELL_CMD_MAIN_ATTR);
