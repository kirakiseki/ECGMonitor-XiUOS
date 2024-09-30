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
* @file:    lv_sensor_info.c
* @brief:   a sensor info application using littleVgl
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#include "lv_sensor_info.h"

static void draw_part_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_param(e);
    /*If the cells are drawn...*/
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);
        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            dsc->label_dsc->flag = LV_TEXT_ALIGN_CENTER;
        }
        /*Make every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}

char *Double2Str(char* buf, double value) {
	sprintf(buf,"%.8f",value);//keep 8 bit float data
	int index = 0;
	int len = strlen(buf);
	for(int i = len-1;i>0;i--) {
		if(buf[i] == '0')
			continue;
        else {
            if (buf[i] == '.') index = i;
            else index = i + 1;
            break;
        }
	}
    buf[index] = '\0';
    return buf;
}

lv_obj_t* lv_ssr_tb;

void lv_sensor_info(void) {
    lv_ssr_tb = lv_table_create(lv_scr_act());
    // lv_obj_remove_style(lv_ssr_tb, NULL, LV_PART_ITEMS | LV_STATE_PRESSED);

    for (uint32_t i = 0; i < NR_VAL_PERLINE; ++i) {
        lv_table_set_cell_value(lv_ssr_tb, 0, 2 * i, "检测量");
        lv_table_set_cell_value(lv_ssr_tb, 0, 2 * i + 1, "数值");
    }

    // fill name
    uint32_t filled_pos = 0;
    uint32_t cur_line_tmp = 0;
    LV_FONT_DECLARE(lvgl_font_chinese);
    while (filled_pos < nr_sensors) {
        cur_line_tmp = 1 + (filled_pos / NR_VAL_PERLINE);
        for (uint32_t i = 0; i < NR_VAL_PERLINE; ++i) {
            if (filled_pos >= nr_sensors) { break; }
            lv_table_set_cell_value(lv_ssr_tb, cur_line_tmp, 2 * i, sensor_names[filled_pos++]);
        }
    }
    lv_obj_set_style_text_font(lv_ssr_tb, &lvgl_font_chinese, 0);

    // set width of cells in table
    for (uint32_t i = 0; i < 2 * NR_VAL_PERLINE; ++i) {
        if (i % 2 == 0) {
            lv_table_set_col_width(lv_ssr_tb, i, 75);
        } else {
            lv_table_set_col_width(lv_ssr_tb, i, 85);
        }
    }

    // fill val
    filled_pos = 0;
    // init val
    for (uint32_t i = 0; i < nr_sensors; ++i) { lv_sensor_vals[i] = 0; }
    char buf[10];
    snprintf(buf, 9, "%.1f", 0);
    while (filled_pos < nr_sensors) {
        for (uint32_t i = 0; i < NR_VAL_PERLINE; ++i) {
            if (filled_pos >= nr_sensors) { break; }
            lv_table_set_cell_value_fmt(lv_ssr_tb, 1 + (filled_pos / NR_VAL_PERLINE),
                1 + 2 * i, "%s %s", buf, seneor_denominations[filled_pos++]);
        }
    }

    lv_obj_set_size(lv_ssr_tb, 480, 272);
    
    lv_obj_set_height(lv_ssr_tb, 272);
    lv_obj_center(lv_ssr_tb);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(lv_ssr_tb, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

void *lvgl_thd_show_sensor_info(void *parameter)
{
    lv_sensor_info();
    PrivMutexCreate(&ssr_val_lock, 0);
    while (1) {
        lv_task_handler();

        sensor_update_table();
    }
}

pthread_t lvgl_task;
static int lvgl_show_sensor_info(void)
{
    pthread_attr_t attr;
    attr.schedparam.sched_priority = 25;
    attr.stacksize = 4096;

    PrivTaskCreate(&lvgl_task, &attr, lvgl_thd_show_sensor_info, NULL);

    return 0;
}
PRIV_SHELL_CMD_FUNCTION(lvgl_show_sensor_info, a littlevgl sensor info show sample, PRIV_SHELL_CMD_MAIN_ATTR);
