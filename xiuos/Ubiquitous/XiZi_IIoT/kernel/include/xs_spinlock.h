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
* @file:    xs_service.h
* @brief:   maroc and switch table definition of kernel switch 
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2020/3/2
*
*/

#ifndef XS_SPINLOCK_H
#define XS_SPINLOCK_H

#include <xs_isr.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARCH_SMP

typedef union 
{
    unsigned long slock;
    struct __arch_tickets 
    {
        unsigned short owner;
        unsigned short next;
    } tickets;
} HwSpinlock;

struct SpinLock
{
    HwSpinlock lock;
};

struct SpinLockfileOps
{                                                                                                                //define spin lock
    struct SpinLock node_lock;                                                    //last spin lock node struct
    void (*SPinLock)(struct SpinLockfileOps  *spinlock);
    void (*UnlockSpinLock)(struct SpinLockfileOps  *spinlock);
    void (*UnlockSpinLockIrqRestore)(struct SpinLockfileOps  *spinlock, x_base level);
    x_base (*SpinLockIrqSave)(struct SpinLockfileOps  *spinlock);
};

extern struct SpinLockfileOps  spinlock;
extern HwSpinlock _CriticalLock;

void InitHwSpinlock(HwSpinlock *lock);
void HwLockSpinlock(HwSpinlock *lock);
void HwUnlockSpinlock(HwSpinlock *lock);
int GetCpuId(void);

#define HW_SPIN_LOCK_INITIALIZER(lockname) {0}
#define HW_SPIN_LOCK_UNLOCKED(lockname) (HwSpinlock) HW_SPIN_LOCK_INITIALIZER(lockname)
#define DEFINE_SPINLOCK(x) HwSpinlock x = HW_SPIN_LOCK_UNLOCKED(x)
#define DECLARE_SPINLOCK(x)

void StartupSecondaryCpu(void);
void ExecSecondaryCpuIdleKtask(void);

#else

#define HwLockSpinlock(lock)     *(lock) = DISABLE_INTERRUPT()
#define HwUnlockSpinlock(lock)   ENABLE_INTERRUPT(*(lock))

#endif



#ifdef ARCH_SMP
struct SpinLock;

void InitSpinLock(struct   SpinLockfileOps  * spinlock);
void _SpinLock(struct   SpinLockfileOps  * spinlock);
void _UnlockSpinLock(struct   SpinLockfileOps  * spinlock);
x_base _SpinLockIrqSave(struct   SpinLockfileOps  * spinlock);
void _UnlockSpinLockIrqRestore(struct SpinLockfileOps  * spinlock, x_base level);

#else
#define InitSpinLock(lock)                 /* nothing */
#define SpinLock(lock)                      CriticalAreaLock()
#define UnlockSpinLock(lock)                    CriticalAreaUnLock()
#define SpinLockIrqSave(lock)              DISABLE_INTERRUPT()
#define UnlockSpinLockIrqRestore(lock, level)  ENABLE_INTERRUPT(level)

#endif




#ifdef __cplusplus
}
#endif

#endif
