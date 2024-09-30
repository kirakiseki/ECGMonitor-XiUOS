/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file winddirection_qs_fx.c
 * @brief qs-fx wind direction example
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2021.12.14
 */

#include <sensor.h>

/**
 * @description: Read a wind direction
 * @return 0
 */
void WindDirectionQsFx(void)
{
    struct SensorQuantity *wind_direction = SensorQuantityFind(SENSOR_QUANTITY_QS_FX_WINDDIRECTION, SENSOR_QUANTITY_WINDDIRECTION);
    SensorQuantityOpen(wind_direction);
    PrivTaskDelay(2000);
    int result = 0;
    for(int i=0;i<2000;i++)
    {
        result = 0;
        PrivTaskDelay(1000);
        result = SensorQuantityReadValue(wind_direction);
        if(result > 0)
            printf("wind direction : %d degree\n", result);
    }
    SensorQuantityClose(wind_direction);
}
