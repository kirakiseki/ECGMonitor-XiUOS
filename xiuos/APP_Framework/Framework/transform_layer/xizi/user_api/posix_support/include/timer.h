/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS  is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
* @file:    timer.h
* @brief:   the function definition of posix soft timer
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2023/3/8
*
*/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../switch_api/user_api.h"

#if !defined(LIB_MUSLLIB)
#include "signal.h"
#endif

#include <time.h>

#ifndef TIMER_TRIGGER_ONCE
#define TIMER_TRIGGER_ONCE      (1 << 0)
#endif
#ifndef TIMER_TRIGGER_PERIODIC
#define TIMER_TRIGGER_PERIODIC  (1 << 1)
#endif

int timer_create(clockid_t clockid, struct sigevent * evp, timer_t * timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *restrict value,
                  struct itimerspec *restrict ovalue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 
