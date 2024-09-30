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
* @file board.h
* @brief define edu-arm32-board init configure and start-up function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-09-08
*/

/*************************************************
File name: board.h
Description: define edu-arm32-board board init function and struct
Others: 
History: 
1. Date: 2021-04-25
Author: AIIT XUOS Lab
Modification: 
1. define edu-arm32-board InitBoardHardware
2. define edu-arm32-board data and bss struct
*************************************************/

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

extern int __heap_start;
extern unsigned int g_service_table_start;
extern unsigned int g_service_table_end;

#define MEMORY_START_ADDRESS    (&__heap_start)
#define HC32F4_SRAM_SIZE         512
#define MEMORY_END_ADDRESS      (0x1FFE0000 + HC32F4_SRAM_SIZE * 1024)

void InitBoardHardware(void);

#endif