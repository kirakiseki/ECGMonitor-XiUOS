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
* @file:    user_timer.c
* @brief:   the priviate user api of soft timer for application 
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2023/3/8
*
*/

#include "user_api.h"

/**
 * This function will create a soft timer.
 *
 * @param name the name of the timer.
 * @param timeout the callback of the timer.
 * @param parameter the parameter of the callback function
 * @param time the timeout time
 * @param trigger_mode the trigger way of the timer
 *
 * @return id on success;ENOMEMORY/ERROR on failure
 */
int32_t UserTimerCreate(const char *name,
                           void (*timeout)(void *parameter),
                           void *parameter,
                           uint32_t time,
                           uint8_t trigger_mode) {
   return (int32_t)KSwitch5(KS_USER_TIMER_CREATE, (uintptr_t)name, (uintptr_t)timeout, (uintptr_t)parameter, (uintptr_t)time, (uintptr_t)trigger_mode);
}

/**
 * This function will delete a timer.
 *
 * @param timer_id the id number of timer.
 * 
 * @return EOK on success;EINVALED on failure
 *
 */
x_err_t UserTimerDelete(int32_t timer_id) {
   return (x_err_t)KSwitch1(KS_USER_TIMER_DELETE, (uintptr_t)timer_id);
}

/**
 * This function will startup a timer.
 *
 * @param timer_id the id number of timer.
 * 
 * @return EOK on success;EINVALED on failure
 *
 */
x_err_t UserTimerStartRun(int32_t timer_id) {
    return (x_err_t)KSwitch1(KS_USER_TIMER_STARTRUN, (uintptr_t)timer_id);
}

/**
 * This function will stop a timer.
 *
 * @param timer_id the id number of timer.
 * 
 * @return EOK on success;EINVALED on failure
 *
 */
x_err_t UserTimerQuitRun(int32_t timer_id) {
   return (x_err_t)KSwitch1(KS_USER_TIMER_QUITRUN, (uintptr_t)timer_id);
}

/**
 * This function will modify the timeout of a timer.
 *
 * @param timer_id the id number of timer.
 * @param ticks timeout ticks
 * 
 * @return EOK on success;EINVALED on failure
 *
 */
x_err_t UserTimerModify(int32_t timer_id, uint32_t ticks) {
   return (x_err_t)KSwitch2(KS_USER_TIMER_MODIFY, (uintptr_t)timer_id, (uintptr_t)ticks);
}
