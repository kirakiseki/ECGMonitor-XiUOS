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
* @file:    pthread.c
* @brief:   posix api of pthread
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/4/20
*
*/
#include <string.h>
#include <stdio.h>
#include "include/pthread.h"

#define DEFAULT_STACK_SIZE  2048
#define DEFAULT_PRIORITY    (KTASK_PRIORITY_MAX/2 + KTASK_PRIORITY_MAX/4)


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
{
    int ret ;
    int pid ;
    char task_name[32] = {0};
    static int utask_id = 0;
    UtaskType task ;

    if (NULL == attr) {
        task.prio = KTASK_PRIORITY_MAX / 2;
        task.stack_size = 4096 ;
    } else {
        task.prio = attr->schedparam.sched_priority ;
        task.stack_size = attr->stacksize ;
    }

    task.func_entry = start_routine ;
    task.func_param = arg;
    snprintf(task_name, sizeof(task_name) - 1, "utask%02d",utask_id++);
    memcpy(task.name , task_name, sizeof(task_name) - 1);

    pid = UserTaskCreate(task);
    if (pid < 0)
      return -1 ;
    
    ret = UserTaskStartup(pid);
    *thread = (pthread_t)(long)pid;

    return ret;

}

int pthread_attr_init(pthread_attr_t *attr)
{
    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t           *attr,
                               struct sched_param const *param)
{
    // NULL_PARAM_CHECK(attr);
    // NULL_PARAM_CHECK(param);

    attr->schedparam.sched_priority = param->sched_priority;

    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size)
{
    attr->stacksize = stack_size;
    return 0;
}

void pthread_exit(void *value_ptr){
    //todo add exit value
    UserTaskQuit();
}

pthread_t pthread_self(void){
    
    pthread_t pthread ;
    pthread = (pthread_t)(long)UserGetTaskID();
    return pthread;
}

int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *pParam)
{
    return 0;
}

int pthread_setschedprio(pthread_t thread, int prio)
{
    //add syscall
    return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return (int)(t1 == t2);
}

int pthread_cancel(pthread_t thread)
{
    return -1;
}

void pthread_testcancel(void)
{
    return;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    return -1;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    return -1;
}

int pthread_join(pthread_t thread, void **retval)
{
    return -1;
}

int pthread_kill(pthread_t thread, int sig)
{
    /* This api should not be used, and will not be supported */
    int32_t *thread_id_tmp = (void *)&thread;
    UserTaskDelete(*thread_id_tmp);
    return -1;
}


