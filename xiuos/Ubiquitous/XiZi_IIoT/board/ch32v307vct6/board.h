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
* @brief define ch32v307 init configure and start-up function
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-09-02
*/

/*************************************************
File name: board.h
Description: define ch32v307 vct6 init configure and start-up function
Others: 
History: 
1. Date: 2022-08-08
Author: AIIT XUOS Lab
Modification: 
1. define ch32v307 vct6 InitBoardHardware
*************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__

#include "ch32v30x.h"
#include <xsconfig.h>

#define CH32V30X_PIN_NUMBERS   100
/* board configuration */
#define SRAM_SIZE  64
#define SRAM_END (0x20000000 + SRAM_SIZE * 1024)

extern int _ebss;
extern int __stack_size;
#define MEMORY_START_ADDRESS    ((void *)&_ebss)
#define MEMORY_END_ADDRESS      (SRAM_END-__stack_size)

void InitBoardHardware(void);

#endif /* __BOARD_H__ */
