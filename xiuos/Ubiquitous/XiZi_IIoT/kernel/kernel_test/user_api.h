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
* @file user_api.h
* @brief define user api function and struct
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-24
*/

#ifndef __XS_USER_API_H__
#define __XS_USER_API_H__

#include <xizi.h>
#include <iot-vfs_posix.h>

#ifdef  SEPARATE_COMPILE
#include <stddef.h>
#include <stdint.h>
#include <kswitch.h>
#if defined(FS_VFS)
#include <iot-vfs.h>
#ifndef TASK_ISOLATION
#include <iot-vfs_posix.h>
#endif
#endif

#include <xs_service.h>

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
	char     name[NAME_NUM_MAX];         
    void     *func_entry;                
    void     *func_param;     
    uint32   stack_size;  
    uint8    prio; 
};
typedef struct utask UtaskType;

typedef void DIR;

int32 UserTaskCreate(UtaskType utask);

x_err_t UserTaskStartup(int32 id);
x_err_t UserTaskDelete(int32 id);
void UserTaskQuit(void);
x_err_t UserTaskDelay(int32 ms);
x_err_t UserGetTaskName(int32 id ,char *name);
int32 UserGetTaskID(void);
uint8 UserGetTaskStat(int32 id);

#ifdef ARCH_SMP
x_err_t UserTaskCoreCombine(int32 id,uint8 core_id);
x_err_t UserTaskCoreUnCombine(int32 id);
uint8 UserGetTaskCombinedCore(int32 id);
uint8 UserGetTaskRunningCore(int32 id);
#endif

x_err_t UserGetTaskErrorstatus(int32 id);
uint8 UserGetTaskPriority(int32 id);


void *UserMalloc(x_size_t size);
void *UserRealloc(void *pointer, x_size_t size);
void *UserCalloc(x_size_t  count, x_size_t size);
void UserFree(void *pointer);

#ifdef KERNEL_MUTEX
int32 UserMutexCreate();
void UserMutexDelete(int32 mutex);
int32 UserMutexObtain(int32 mutex, int32 wait_time);
int32 UserMutexAbandon(int32 mutex);
#endif


#ifdef KERNEL_SEMAPHORE
typedef int32  sem_t;
sem_t UserSemaphoreCreate(uint16 val);
x_err_t UserSemaphoreDelete(sem_t sem);
x_err_t UserSemaphoreObtain(sem_t sem, int32 wait_time);
x_err_t UserSemaphoreAbandon(sem_t sem);
x_err_t UserSemaphoreSetValue(sem_t sem, uint16 val);
#endif


#ifdef KERNEL_EVENT
EventIdType UserEventCreate(uint8 flag);
void UserEventDelete(EventIdType event);
x_err_t UserEventTrigger(EventIdType event, uint32 set);
x_err_t UserEventProcess(EventIdType event, uint32 set, uint8 option, 
                         int32   wait_time, uint32 *Recved);
x_err_t UserEventReinit(EventIdType event);
#endif


#ifdef KERNEL_MESSAGEQUEUE
int32 UserMsgQueueCreate(x_size_t   msg_size, x_size_t   max_msgs);
x_err_t UserMsgQueueDelete(int32 mq );
x_err_t UserMsgQueueSendwait(int32 mq, const void *buffer,
                                      x_size_t   size, int32  wait_time);
x_err_t UserMsgQueueSend(int32 mq, const void *buffer, x_size_t size);
x_err_t UserMsgQueueUrgentSend(int32 mq, const void *buffer, x_size_t size);
x_err_t UserMsgQueueRecv(int32 mq, void *buffer, x_size_t  size,int32 wait_time);
x_err_t UserMsgQueueReinit(int32 mq);
#endif

int open(const char *path, int flags, ...);
int read(int fd, void *buf, size_t len);
int write(int fd, const void *buf, size_t len);
int close(int fd);
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
#endif

int UserPrintf(const char *fmt, ...);

#else
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

x_err_t UserGetTaskName(int32_t id ,char *name);
int32_t UserGetTaskID(void);
uint8_t UserGetTaskStat(int32_t id);

#ifdef ARCH_SMP
#define UserTaskCoreCombine      KTaskCoreCombine
#define UserTaskCoreUnCombine    KTaskCoreUnCombine

uint8_t UserGetTaskCombinedCore(int32_t id);
uint8_t UserGetTaskRunningCore(int32_t id);
#endif

x_err_t UserGetTaskErrorstatus(int32_t id);
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

#define UserPrintf               KPrintf
#endif

#endif