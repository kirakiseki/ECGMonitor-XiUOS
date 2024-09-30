/*
* Copyright (c) 2022 AIIT XUOS Lab
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
 * @file lt768.h
 * @brief lt768 register relative driver, inherit from Levetop Electronics
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.9.19
 */

#ifndef _LT768_LEARN_H
#define _LT768_LEARN_H
#include "nuttx/lcd/if_port.h"
#include <stdlib.h>

void Display_RGB(void);
void Display_Picture(void);
void Display_PIP(void);
void Display_Internal_Font(void);
void Display_Outside_Font(void);
void Display_Triangle(void);
void Tsst(void);

#endif
