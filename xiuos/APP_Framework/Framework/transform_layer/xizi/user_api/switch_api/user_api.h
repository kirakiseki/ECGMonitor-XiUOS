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
* @file:    user_api.h
* @brief:   the priviate user api for application
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/4/20
*
*/

#ifndef XS_USER_API_H
#define XS_USER_API_H

#include <xsconfig.h>
#include <stddef.h>
#include <stdint.h>
#include <libc.h>


#ifdef  SEPARATE_COMPILE

#include "../../../../../../Ubiquitous/XiZi_IIoT/arch/kswitch.h"

#define TASK_INFO      1
#define MEM_INFO       2
#define SEM_INFO       3
#define EVENT_INFO     4
#define MUTEX_INFO     5
#define MEMPOOL_INFO   6
#define MSGQUEUE_INFO  7
#define DEVICE_INFO    8
#define TIMER_INFO     9

int UserPrintInfo(unsigned long i);

struct utask
{
	char        name[NAME_NUM_MAX];         
    void        *func_entry;                
    void        *func_param;     
    int32_t     stack_size;  
    uint8_t     prio; 
};
typedef struct utask UtaskType;

typedef void DIR;

int32_t UserTaskCreate(UtaskType utask);

long UserTaskStartup(int32_t id);
long UserTaskDelete(int32_t id);
long UserTaskSearch(void);
void UserTaskQuit(void);
long UserTaskDelay(int32_t ms);
long UserGetTaskName(int32_t id ,char *name);
int32_t UserGetTaskID(void);
uint8_t UserGetTaskStat(int32_t id);

#ifdef ARCH_SMP
long UserTaskCoreCombine(int32_t id,uint8_t core_id);
long UserTaskCoreUnCombine(int32_t id);
uint8_t UserGetTaskCombinedCore(int32_t id);
uint8_t UserGetTaskRunningCore(int32_t id);
#endif

long UserGetTaskErrorstatus(int32_t id);
uint8_t UserGetTaskPriority(int32_t id);


void *UserMalloc(size_t size);
void *UserRealloc(void *pointer, size_t size);
void *UserCalloc(size_t  count, size_t size);
void UserFree(void *pointer);

#ifdef KERNEL_MUTEX
int32_t UserMutexCreate();
void UserMutexDelete(int32_t mutex);
int32_t UserMutexObtain(int32_t mutex, int32_t wait_time);
int32_t UserMutexAbandon(int32_t mutex);
#endif


#ifdef KERNEL_SEMAPHORE
typedef int32  sem_t;
sem_t UserSemaphoreCreate(uint16_t val);
long UserSemaphoreDelete(sem_t sem);
long UserSemaphoreObtain(sem_t sem, int32_t wait_time);
long UserSemaphoreAbandon(sem_t sem);
long UserSemaphoreSetValue(sem_t sem, uint16_t val);
#endif


#ifdef KERNEL_EVENT
typedef int32 EventIdType;
EventIdType UserEventCreate(uint8_t flag);
void UserEventDelete(EventIdType event);
long UserEventTrigger(EventIdType event, uint32_t set);
long UserEventProcess(EventIdType event, uint32_t set, uint8_t option, 
                         int32_t   wait_time, uint32_t *Recved);
long UserEventReinit(EventIdType event);
#endif


#ifdef KERNEL_MESSAGEQUEUE
int32_t UserMsgQueueCreate(size_t   msg_size, size_t   max_msgs);
long UserMsgQueueDelete(int32_t mq );
long UserMsgQueueSendwait(int32_t mq, const void *buffer,
                                      size_t   size, int32_t  wait_time);
long UserMsgQueueSend(int32_t mq, const void *buffer, size_t size);
long UserMsgQueueUrgentSend(int32_t mq, const void *buffer, size_t size);
long UserMsgQueueRecv(int32_t mq, void *buffer, size_t  size,int32_t wait_time);
long UserMsgQueueReinit(int32_t mq);
#endif

#ifdef KERNEL_SOFTTIMER
int32_t UserTimerCreate(const char *name, void (*timeout)(void *parameter), void *parameter, uint32_t time, uint8_t trigger_mode);
long UserTimerDelete(int32_t timer_id);
long UserTimerStartRun(int32_t timer_id);
long UserTimerQuitRun(int32_t timer_id);
long UserTimerModify(int32_t timer_id, uint32_t ticks);
#endif

int open(const char *path, int flags, ...);
int read(int fd, void *buf, size_t len);
int write(int fd, const void *buf, size_t len);
int close(int fd);
int ioctl(int fd, int cmd, void *args);
off_t lseek(int fd, off_t offset, int whence);
int rename(const char *from, const char *to);
int unlink(const char *path);
int stat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int fsync(int fd);
int ftruncate(int fd, off_t length);

int mkdir(const char *path, mode_t mode);
DIR *opendir(const char *path);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);
int rmdir(const char *path);
int chdir(const char *path);
char *getcwd(char *buf, size_t size);
long telldir(DIR *dirp);
void seekdir(DIR *dirp, off_t offset);
void rewinddir(DIR *dirp);

#ifdef FS_VFS
struct statfs {
    size_t f_bsize;
    size_t f_blocks;
    size_t f_bfree;
};

int statfs(const char *path, struct statfs *buf);

/* NOTE!!!: when cutting out file system, the 'printf' function can not output angthing */
int Userprintf(const char *fmt, ...);

#endif
 

#else

#include <xizi.h>

#ifdef FS_VFS
#include <iot-vfs_posix.h>
#endif

struct utask
{
	char        name[NAME_NUM_MAX];         
    void        *func_entry;                
    void        *func_param;     
    int32_t     stack_size;  
    uint8_t     prio; 
};
typedef struct utask UtaskType;
int32_t UserTaskCreate(UtaskType utask);

#define UserTaskStartup          StartupKTask
#define UserTaskDelete           KTaskDelete
#define UserTaskQuit             KTaskQuit
#define UserTaskDelay            MdelayKTask
#define UserTaskSearch           UTaskSearch

long UserGetTaskName(int32_t id ,char *name);
int32_t UserGetTaskID(void);
uint8_t UserGetTaskStat(int32_t id);

#ifdef ARCH_SMP
#define UserTaskCoreCombine      KTaskCoreCombine
#define UserTaskCoreUnCombine    KTaskCoreUnCombine

uint8_t UserGetTaskCombinedCore(int32_t id);
uint8_t UserGetTaskRunningCore(int32_t id);
#endif

long UserGetTaskErrorstatus(int32_t id);
uint8_t UserGetTaskPriority(int32_t id);

#define UserMalloc               x_malloc
#define UserRealloc              x_realloc
#define UserCalloc               x_calloc
#define UserFree                 x_free

#ifdef KERNEL_MUTEX
#define UserMutexCreate          KMutexCreate
#define UserMutexDelete          KMutexDelete
#define UserMutexObtain          KMutexObtain
#define UserMutexAbandon         KMutexAbandon
#endif


#ifdef KERNEL_SEMAPHORE
#define UserSemaphoreCreate      KSemaphoreCreate
#define UserSemaphoreDelete      KSemaphoreDelete
#define UserSemaphoreObtain      KSemaphoreObtain
#define UserSemaphoreAbandon     KSemaphoreAbandon
#define UserSemaphoreSetValue    KSemaphoreSetValue
#endif

#ifdef KERNEL_EVENT
#define UserEventCreate          KEventCreate
#define UserEventDelete          KEventDelete
#define UserEventTrigger         KEventTrigger
#define UserEventProcess         KEventProcess
#endif

#ifdef KERNEL_MESSAGEQUEUE
#define UserMsgQueueCreate       KCreateMsgQueue
#define UserMsgQueueDelete       KDeleteMsgQueue
#define UserMsgQueueSendwait     KMsgQueueSendwait
#define UserMsgQueueSend         KMsgQueueSend
#define UserMsgQueueUrgentSend   KMsgQueueUrgentSend
#define UserMsgQueueRecv         KMsgQueueRecv
#define UserMsgQueueReinit       KMsgQueueReinit
#endif

#ifdef KERNEL_SOFTTIMER
int32_t UserTimerCreate(const char *name, void (*timeout)(void *parameter), void *parameter, uint32_t time, uint8_t trigger_mode);
long UserTimerDelete(int32_t timer_id);
long UserTimerStartRun(int32_t timer_id);
long UserTimerQuitRun(int32_t timer_id);
long UserTimerModify(int32_t timer_id, uint32_t ticks);
#endif

#define UserPrintf               KPrintf

#endif

#endif