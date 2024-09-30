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
* @file:    test_lcd.c
* @brief:   a application of lcd function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/17
*/
#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

#define GRAPHIC_CTRL_RECT_UPDATE 0x00
#define LCD_STRING_TYPE 0
#define LCD_DOT_TYPE 1
#define LCD_FONT_RECT_WIDTH 150
#define LCD_FONT_RECT_HEIGHT 50
#define NULL_PARAMETER 0

void TestLcd(void)
{
    int lcd_fd = PrivOpen(EDU_LCD_DEV_DRIVER, O_RDWR);
    if (lcd_fd < 0)
    {
        printf("open lcd fd error:%d\n", lcd_fd);
        return;
    }

    LcdWriteParam graph_param;

    // black
    uint16* color_select = malloc(sizeof(uint16) * 321 * 321);
    memset(color_select, 0x00, sizeof(uint16) * 321 * 321);
    graph_param.type = LCD_DOT_TYPE;
    graph_param.pixel_info.x_startpos = 0;
    graph_param.pixel_info.y_startpos = 0;
    graph_param.pixel_info.x_endpos = 320;
    graph_param.pixel_info.y_endpos = 320;
    graph_param.pixel_info.pixel_color = color_select;
    PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
    free(color_select);

    // draw text
    graph_param.type = LCD_STRING_TYPE;
    graph_param.string_info.x_pos = 0;
    graph_param.string_info.y_pos = 0;
    graph_param.string_info.width = 250;
    graph_param.string_info.height = 24;
    graph_param.string_info.font_size = 24;
    graph_param.string_info.back_color = 0xFFFF;
    graph_param.string_info.font_color = 0x0000;
    graph_param.string_info.addr = "hello_world!";
    PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);

    // draw line
    color_select = malloc(sizeof(uint16) * 1 * 320);
    for (int i = 0; i < 320; i++)
    {
        color_select[i] = 0xF800;
    }
    for (int i = 1; i <= 5; i++)
    {
        graph_param.type = LCD_DOT_TYPE;
        graph_param.pixel_info.x_startpos = 0;
        graph_param.pixel_info.y_startpos = i * 50;
        graph_param.pixel_info.x_endpos = 319;
        graph_param.pixel_info.y_endpos = i * 50;
        graph_param.pixel_info.pixel_color = color_select;
        PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
    }
    free(color_select);

    PrivClose(lcd_fd);
}

PRIV_SHELL_CMD_FUNCTION(TestLcd, a lcd test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif