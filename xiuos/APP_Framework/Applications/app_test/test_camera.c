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
* @file:    test_camera.c
* @brief:   a application of camera function
* @version: 1.1
* @author:  AIIT XUOS Lab
* @date:    2022/12/7
*/

#include <stdio.h>
#include <string.h>
#include <transform.h>
#ifdef ADD_XIZI_FEATURES

static uint16_t image_buff[384000];

void TestCamera(int argc, char *argv[])
{
    int frame_counter = 10000;
    if (argc > 1)
    {
        frame_counter = atoi(argv[1]);
    }
    printf("This test will refresh %d frames\n", frame_counter);

    int camera_fd = PrivOpen(CAMERA_DEV_DRIVER, O_RDWR);
    if (camera_fd < 0)
    {
        printf("open camera fd error:%d\n", camera_fd);
        return;
    }
    int lcd_fd = PrivOpen(CAMERA_LCD_DEV_DRIVER, O_RDWR);
    if (lcd_fd < 0)
    {
        printf("open lcd fd error:%d\n", lcd_fd);
        return;
    }

    //configure the camera's output address
    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = CAMERA_TYPE;
    struct CameraCfg camera_cfg ={
        .gain_manu_enable = 0,
        .gain = 0xFF,
        .window_w = 800,
        .window_h = 600,
        .output_w = IMAGE_WIDTH,
        .output_h = IMAGE_HEIGHT,
        .window_xoffset = 0,
        .window_yoffset = 0
    };
    ioctl_cfg.args = &camera_cfg;
    if (0 != PrivIoctl(camera_fd, OPE_CFG, &ioctl_cfg))
    {
        printf("camera pin fd error %d\n", camera_fd);
        PrivClose(camera_fd);
        return;
    }

    ioctl_cfg.args = (void *)image_buff;

    if (0 != PrivRead(camera_fd, image_buff, NULL_PARAMETER))
    {
        printf("camera pin fd error %d\n", camera_fd);
        PrivClose(camera_fd);
        return;
    }

    printf("address buff is %x\n", image_buff);


    LcdWriteParam graph_param;
    graph_param.type = LCD_DOT_TYPE;

    //clear the LCD
    uint16_t back_color[LCD_SIZE];
    memset(back_color,0,sizeof(back_color));
    for (int i = 0; i < LCD_SIZE; i++)
    {
        graph_param.pixel_info.pixel_color = &back_color;
        graph_param.pixel_info.x_startpos = 0;
        graph_param.pixel_info.y_startpos = i;
        graph_param.pixel_info.x_endpos = LCD_SIZE -1;
        graph_param.pixel_info.y_endpos = i;
        PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
    }

    //refresh the LCD using photo of camera
    while (frame_counter--)
    {
        for (int i = 0; i < IMAGE_HEIGHT; i++)
        {
            graph_param.pixel_info.pixel_color = image_buff + i * IMAGE_WIDTH;
            graph_param.pixel_info.x_startpos = 0;
            graph_param.pixel_info.y_startpos = i + (LCD_SIZE - IMAGE_HEIGHT) / 2;
            graph_param.pixel_info.x_endpos = IMAGE_WIDTH - 1;
            graph_param.pixel_info.y_endpos = i + (LCD_SIZE - IMAGE_HEIGHT) / 2;
            PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
        }
    }

    // close test
    PrivClose(lcd_fd);
    PrivClose(camera_fd);
    printf("The camera test is finished successfully\n");
}

PRIV_SHELL_CMD_FUNCTION(TestCamera, a camera test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif