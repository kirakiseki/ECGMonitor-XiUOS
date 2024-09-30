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
* @file:    lv_sensor_info.h
* @brief:   a sensor info application using littleVgl
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#ifndef __LVGL_SENSOR_INFO_H__
#define __LVGL_SENSOR_INFO_H__

#include <lvgl.h>
#include <string.h>

#define nr_sensors 18
extern lv_obj_t* lv_ssr_tb;
pthread_mutex_t ssr_val_lock;

// number of sensor values showed in one line
#define NR_VAL_PERLINE 3

static char* sensor_names[nr_sensors] = {
    "臭氧", "二氧化碳", "二氧化硫", "二氧化氮", "氨气",
    "TVOC", "甲醛", "乙醇", "甲烷", "氧气", "AQS", "PM1.0/2.5/10",
    "温度", "湿度",
    "风速", "风向", "气压", "噪音"
};

static char* seneor_denominations[nr_sensors] = {
    "ppb", "ppm", "ppb", "ppb", "ppm",
    "ppm", "ppm", "ppm", "%VOL", "%VOL", "ug/m³", "ug/m³",
    "°C", "%RH",
    "m/s", "m/s", "mbar", "dB(A)"
};

static double lv_sensor_vals[nr_sensors];

enum sensor_type {
    O3 = 0, // 臭氧
    CO2,  // 二氧化碳
    SO2,  // 二氧化硫
    NO2,  // 二氧化氮
    N2,  // 氨气
    TVOC,  
    FORMALDEHYDE,  // 甲醛
    ALCOHOL,  // 乙醇
    METHANE,  // 甲烷
    O2,  // 氧气
    AQS,
    PM,  // PM1.0/2.5
    TEMPERATURE,  // 温度
    HUMIDITY,  // 湿度 
    WIND_SPEED,  // 风速
    WIND_DIRECTION,  //风向
    PRESURE,  // 压力
    NOISE  // 噪音
};

void lv_sensor_info(void);
bool sensor_update_val(double, enum sensor_type);
char* Double2Str(char* buf, double value);
void sensor_update_table();

#endif