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
* @file:    test_timer.c
* @brief:   a application of soft timer function
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2023/03/09
*/

#include <transform.h>

void TimerFunction(union sigval sig_val)
{
    static int cnt = 0;
    printf("%s cnt %d\n", __func__, cnt++);
}

void TestTimer(void)
{
    int ret = 0;
    int timer_flags;
    timer_t timer_id;
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));

    timer_flags = TIMER_TRIGGER_PERIODIC;
    
    evp.sigev_notify = SIGEV_THREAD; 
    evp.sigev_notify_function = TimerFunction;
    evp.sigev_notify_attributes = &timer_flags;

    ret = timer_create(CLOCK_REALTIME, &evp, &timer_id);
    if (ret < 0) {
        printf("%s create timer failed ret %d\n", __func__, ret);
        return;
    }

    struct itimerspec value; 
    //active time interval
    value.it_interval.tv_sec = 2; 
    value.it_interval.tv_nsec = 0;

    //first timer set time 
    value.it_value.tv_sec = 2; 
    value.it_value.tv_nsec = 0; 

    ret = timer_settime(timer_id, 1, &value, NULL);
    if (ret < 0) { 
        printf("%s set timer time failed ret %d\n", __func__, ret);
        return;
    }

    printf("%s success\n", __func__);
}
PRIV_SHELL_CMD_FUNCTION(TestTimer, soft timer test, PRIV_SHELL_CMD_MAIN_ATTR);

