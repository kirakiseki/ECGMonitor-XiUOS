/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _OV2640_H
#define _OV2640_H

#include <stdint.h>

#define OV2640_ADDR         0x60

struct CameraCfg
{
    uint16_t window_w;
    uint16_t window_h;
    uint16_t window_xoffset;
    uint16_t window_yoffset;
    uint16_t output_w;
    uint16_t output_h;
    uint8_t gain;
    uint8_t gain_manu_enable;
};

int ov2640_init(void);
int ov2640_read_id(uint16_t *manuf_id, uint16_t *device_id);
int SensorConfigure(struct CameraCfg* cfg_info);
void ov2640_jpeg_mode(void);
void ov2640_rgb565_mode(void);
void ov2640_set_auto_exposure(uint8_t level);
void ov2640_set_light_mode(uint8_t mode);
void ov2640_set_color_saturation(uint8_t sat);
void ov2640_set_brightness(uint8_t bright);
void ov2640_set_contrast(uint8_t contrast);
void ov2640_set_special_effects(uint8_t eft);
void ov2640_set_window_size(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
uint8_t ov2640_set_image_out_size(uint16_t width,uint16_t height);
uint8_t ov2640_set_image_window_size(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height);
uint8_t ov2640_set_image_size(uint16_t width ,uint16_t height);

#endif /* _OV2640_H */
