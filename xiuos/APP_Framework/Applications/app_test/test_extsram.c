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
* @file:    test_extsram.c
* @brief:   a extsram test application
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*
*/

#include <transform.h>
#include "stdio.h"

/* parameters for sram peripheral */
// /* stm32f4 Bank3:0X68000000 */
// #define SRAM_BANK_ADDR       ((uint32_t)0X68000000)

/* OK-1052 semc:0X80000000 */
#define SRAM_BANK_ADDR       ((uint32_t)0X80000000)

/* data width: 8, 16, 32 */
#define SRAM_DATA_WIDTH      16

/* sram size */
#define SRAM_SIZE            ((uint32_t)0x2000000)

#define TICK_PER_SECOND 100

int ExtsramTest(void)
{
    int i = 0;
    uint32_t start_time = 0, time_cast = 0;
#if SRAM_DATA_WIDTH == 8
    char data_width = 1;
    uint8_t data = 0;
#elif SRAM_DATA_WIDTH == 16
    char data_width = 2;
    uint16_t data = 0;
#else
    char data_width = 4;
    uint32_t data = 0;
#endif

    /* write data */
    printf("Writing the %ld bytes data, waiting....\n", SRAM_SIZE);
    start_time = PrivGetTickTime();
    for (i = 0; i < SRAM_SIZE / data_width; i++)
    {
#if SRAM_DATA_WIDTH == 8
        *(volatile uint8_t *)(SRAM_BANK_ADDR + i * data_width) = (uint8_t)0x55;
#elif SRAM_DATA_WIDTH == 16
        *(volatile uint16_t *)(SRAM_BANK_ADDR + i * data_width) = (uint16_t)0x5555;
#else
        *(volatile uint32_t *)(SRAM_BANK_ADDR + i * data_width) = (uint32_t)0x55555555;
#endif
    }
    time_cast = PrivGetTickTime() - start_time;
    printf("Write data success, total time: %ld.%03ldS.\n", time_cast / TICK_PER_SECOND,
          time_cast % TICK_PER_SECOND / ((TICK_PER_SECOND * 1 + 999) / 1000));

    /* read data */
    printf("start Reading and verifying data, waiting....\n");
    for (i = 0; i < SRAM_SIZE / data_width; i++)
    {
#if SRAM_DATA_WIDTH == 8
        data = *(volatile uint8_t *)(SRAM_BANK_ADDR + i * data_width);
        if (data != 0x55)
        {
            printf("SRAM test failed!");
            break;
        }
#elif SRAM_DATA_WIDTH == 16
        data = *(volatile uint16_t *)(SRAM_BANK_ADDR + i * data_width);
        if (data != 0x5555)
        {
            printf("SRAM test failed! data = 0x%x\n",data);
            break;
        }
#else
        data = *(volatile uint32_t *)(SRAM_BANK_ADDR + i * data_width);
        if (data != 0x55555555)
        {
            printf("SRAM test failed!\n");
            break;
        }
#endif
    }

    if (i >= SRAM_SIZE / data_width)
    {
        printf("SRAM test success!\n");
    }

    return 0;
}
PRIV_SHELL_CMD_FUNCTION(ExtsramTest, a extsram test sample, PRIV_SHELL_CMD_MAIN_ATTR);
