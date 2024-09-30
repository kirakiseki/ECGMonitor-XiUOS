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
 * @file xiuos.c
 * @brief Converts the framework interface to an operating system interface
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.06.07
 */

#include <transform.h>

/**************************mutex***************************/
/* private mutex API */
int PrivMutexCreate(pthread_mutex_t *p_mutex, const pthread_mutexattr_t *attr)
{
    return pthread_mutex_init(p_mutex, attr);
}

int PrivMutexDelete(pthread_mutex_t *p_mutex)
{
    return pthread_mutex_destroy(p_mutex);
}

int PrivMutexObtain(pthread_mutex_t *p_mutex)
{
    return pthread_mutex_lock(p_mutex);
}

int PrivMutexAbandon(pthread_mutex_t *p_mutex)
{
    return pthread_mutex_unlock(p_mutex);
}

/**********************semaphore****************************/
int PrivSemaphoreCreate(sem_t *sem, int pshared, unsigned int value)
{
    return sem_init(sem, pshared, value);
}

int PrivSemaphoreDelete(sem_t *sem)
{
    return sem_destroy(sem);
}

int PrivSemaphoreObtainWait(sem_t *sem, const struct timespec *abstime)
{
    return sem_timedwait(sem, abstime);
}

int PrivSemaphoreObtainNoWait(sem_t *sem)
{
    return sem_trywait(sem);
}

int PrivSemaphoreAbandon(sem_t *sem)
{
    return sem_post(sem);
}

/**********************event****************************/
#ifndef SEPARATE_COMPILE
int PrivEventCreate(uint8_t flag)
{
    return UserEventCreate(flag);
}

int PrivEvenDelete(int event)
{
    UserEventDelete(event);
    return 1;
}

int PrivEvenTrigger(int event, uint32_t set)
{
    return UserEventTrigger(event, set);
}

int PrivEventProcess(int event, uint32_t set, uint8_t option, int32_t wait_time, unsigned int *Recved)
{
    return UserEventProcess(event, set, option, wait_time, Recved);
}
#endif

/**************************task*************************/
int PrivTaskCreate(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
{
    return pthread_create(thread, attr, start_routine, arg);
}

int PrivTaskStartup(pthread_t *thread)
{
    return 0;
}
#ifdef SEPARATE_COMPILE
/* private API of xiuos to search the first user task in manage list */
int PrivUserTaskSearch(void)
{
    return UserTaskSearch();
}
#endif

int PrivTaskDelete(pthread_t thread, int sig)
{
    return pthread_kill(thread, sig);
}

void PrivTaskQuit(void *value_ptr)
{
    pthread_exit(value_ptr);
}

int PrivTaskDelay(int32_t ms)
{
    UserTaskDelay(ms);
}
#ifndef SEPARATE_COMPILE
uint32_t PrivGetTickTime()
{
    return CalculateTimeMsFromTick(CurrentTicksGain());
}
#endif

/******************Soft Timer*********************/
int PrivTimerCreate(clockid_t clockid, struct sigevent * evp, timer_t * timerid)
{
    return timer_create(clockid, evp, timerid);
}

int PrivTimerDelete(timer_t timerid)
{
    return timer_delete(timerid);
}

int PrivTimerStartRun(timer_t timerid)
{
    return UserTimerStartRun(timerid);
}

int PrivTimerQuitRun(timer_t timerid)
{
    return UserTimerQuitRun(timerid);
}

int PrivTimerModify(timer_t timerid, int flags, const struct itimerspec *restrict value,
                  struct itimerspec *restrict ovalue)
{
    return timer_settime(timerid, flags, value, ovalue);
}

/*************************************************/

/*********************fs**************************/
#ifdef FS_VFS
/************************Driver Posix Transform***********************/
int PrivOpen(const char *path, int flags)
{
    return open(path, flags);
}

int PrivClose(int fd)
{
    return close(fd);
}

int PrivRead(int fd, void *buf, size_t len)
{    
    return read(fd, buf, len);
}

int PrivWrite(int fd, const void *buf, size_t len)
{   
    return write(fd, buf, len);
}

static int PrivSerialIoctl(int fd, int cmd, void *args)
{
    struct SerialDataCfg *serial_cfg = (struct SerialDataCfg *)args;
    return ioctl(fd, cmd, serial_cfg);
}

static int PrivPinIoctl(int fd, int cmd, void *args)
{
    struct PinParam *pin_cfg = (struct PinParam *)args;

    return ioctl(fd, cmd, pin_cfg);
}

static int PrivLcdIoctl(int fd, int cmd, void *args)
{
    struct DeviceLcdInfo *lcd_cfg = (struct DeviceLcdInfo *)args;

    return ioctl(fd, cmd, lcd_cfg);
}

int PrivIoctl(int fd, int cmd, void *args)
{
    int ret;
    struct PrivIoctlCfg *ioctl_cfg = (struct PrivIoctlCfg *)args;
    switch (ioctl_cfg->ioctl_driver_type)
    {
    case SERIAL_TYPE:
        ret = PrivSerialIoctl(fd, cmd, ioctl_cfg->args);
        break;
    case PIN_TYPE:
        ret = PrivPinIoctl(fd, cmd, ioctl_cfg->args);
        break;
    case LCD_TYPE:
        ret = PrivLcdIoctl(fd, cmd, ioctl_cfg->args);
        break;
    case SPI_TYPE:
    case I2C_TYPE:
    case RTC_TYPE:
    case ADC_TYPE:
    case DAC_TYPE:
    case WDT_TYPE:
    case CAMERA_TYPE:
    case KPU_TYPE:
    case TIME_TYPE:
    case FLASH_TYPE:
    case CAN_TYPE:
        ret = ioctl(fd, cmd, ioctl_cfg->args);
        break;
    default:
        break;
    }

    return ret;
}
#endif
/********************memory api************/
void *PrivMalloc(size_t size)
{
    return UserMalloc(size);
}

void *PrivRealloc(void *pointer, size_t size)
{
    return UserRealloc(pointer, size);
}

void *PrivCalloc(size_t  count, size_t size)
{
    return UserCalloc(count, size);
}

void PrivFree(void *pointer)
{
    UserFree(pointer);
}

