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
* @file:    lv_sensor_update_info.c
* @brief:   a sensor update info application using littleVgl
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#include "lv_sensor_info.h"

uint32_t lv_ssr_map_idx(enum sensor_type st) {
    uint32_t idx;
    switch (st) {
    case O3: {idx = 0; break;}
    case CO2: {idx = 1; break;}
    case SO2: {idx = 2; break;}
    case NO2: {idx = 3; break;}
    case N2: {idx = 4; break;}
    case TVOC: {idx = 5; break;}
    case FORMALDEHYDE: {idx = 6; break;}
    case ALCOHOL: {idx = 7; break;}
    case METHANE: {idx = 8; break;}
    case O2: {idx = 9; break;}
    case AQS: {idx = 10; break;}
    case PM: {idx = 11; break;}
    case TEMPERATURE: {idx = 12; break;}
    case HUMIDITY: {idx = 13; break;}
    case WIND_SPEED: {idx = 14; break;}
    case WIND_DIRECTION: {idx = 15; break;}
    case PRESURE: {idx = 16; break;}
    case NOISE: {idx = 17; break;}
    default: {idx = -1; break;}
    }
    return idx;
}

bool sensor_update_val(double val, enum sensor_type st) {
    uint32_t idx = lv_ssr_map_idx(st);
    if (idx >= nr_sensors || lv_ssr_tb == NULL) { return false; }
    PrivMutexObtain(&ssr_val_lock);
    lv_sensor_vals[idx] = val;
    PrivMutexAbandon(&ssr_val_lock);
    return true;
}

/**
 *@brief update cell vals in lv_ssr_tb, 
 *       note that this function can only be called in lv_ssr_tb control thread 
 *
 */
void sensor_update_table() {
    uint32_t filled_pos = 0;
    char buf[10] = { 0 };
    PrivMutexObtain(&ssr_val_lock);
    while (filled_pos < nr_sensors) {
        for (uint32_t i = 0; i < NR_VAL_PERLINE; ++i) {
            if (filled_pos >= nr_sensors) { break; }
            snprintf(buf, 9, "%.1f", lv_sensor_vals[filled_pos]);
            lv_table_set_cell_value_fmt(lv_ssr_tb, 1 + (filled_pos / NR_VAL_PERLINE),
                1 + 2 * i, "%s %s", buf, seneor_denominations[filled_pos++]);
        }
    }
    PrivMutexAbandon(&ssr_val_lock);
}

