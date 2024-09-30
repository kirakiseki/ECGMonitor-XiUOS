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
* @file:    lv_init.c
* @brief:   init littleVgl
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#include <lvgl.h>
#define DBG_TAG    "LVGL"
#define DBG_LVL    DBG_INFO

#ifndef PKG_USING_LVGL_DISP_DEVICE
#include <lv_port_disp_template.h>
#endif

#ifndef PKG_USING_LVGL_INDEV_DEVICE
#include <lv_port_indev_template.h>
#endif
extern void lv_port_disp_init(void);
extern void lv_port_indev_init(void);
#if LV_USE_LOG && LV_LOG_PRINTF
static void lv_rt_log(const char *buf)
{
    printf(buf);
    printf("\n");
}
#endif

int lv_port_init(void)
{
#if LV_USE_LOG && LV_LOG_PRINTF
    lv_log_register_print_cb(lv_rt_log);
#endif
    lv_init();

#ifndef PKG_USING_LVGL_DISP_DEVICE
    lv_port_disp_init();
#endif

#ifndef PKG_USING_LVGL_INDEV_DEVICE
    lv_port_indev_init();
#endif

    return 0;
}

