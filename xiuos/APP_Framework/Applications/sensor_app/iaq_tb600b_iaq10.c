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
 * @file iaq_tb600b_iaq10.c
 * @brief iaq10 example
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.12.14
 */

#include <sensor.h>

/**
 * @description: Read a iaq
 * @return 0
 */
void IaqTb600bIaq10(void)
{
    struct SensorQuantity *iaq = SensorQuantityFind(SENSOR_QUANTITY_TB600B_IAQ, SENSOR_QUANTITY_IAQ);
    SensorQuantityOpen(iaq);
    int32_t result = 0;

    result = SensorQuantityReadValue(iaq);

    printf("Gas concentration is : %dppb\n", result);
    SensorQuantityClose(iaq);
}