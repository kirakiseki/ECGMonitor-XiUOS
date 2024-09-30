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
* @file:    task.h
* @brief:   file task.h
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2023/05/18
*
*/

#ifndef __TASK_H__
#define __TASK_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 任务状态枚举类型
typedef enum {
    TASK_CREATED,    // 初始状态
    TASK_READY,      // 就绪状态
    TASK_RUNNING,    // 运行状态
    TASK_BLOCKED,    // 阻塞状态
} task_state_t; 


// 任务控制块结构体
typedef struct tcb {
    uint32_t *stack_ptr;                    // 任务堆栈指针
    uint32_t *stack_bottom;                 // 任务堆栈底部指针
    uint32_t stack_size;                    // 任务堆栈大小
    task_state_t state;                     // 任务状态
    int priority;                           // 任务优先级
    struct tcb *next;                       // 指向下一个任务控制块的指针
    struct message_queue *message_queue;    // 消息队列
} tcb_t;


// 消息结构体
typedef struct message {
    int type;    // 消息类型
    void *data;  // 消息数据指针
} message_t;


// 消息队列结构体
typedef struct message_queue {
    message_t *buffer;       // 消息缓冲区
    int capacity;            // 缓冲区容量
    int count;               // 当前缓冲区中的消息数量
    int head;                // 队头指针
    int tail;                // 队尾指针
    semaphore_t *semaphore;  // 信号量，用于实现消息队列的同步机制
} message_queue_t;


// 信号量结构体
typedef struct semaphore {
    int count;              // 计数器
    tcb_t *wait_list_head;  // 指向等待信号量的任务控制块的指针
    mutex_t mutex;          // 互斥锁，用于保护信号量和等待队列的访问
} semaphore_t;


// 互斥锁结构体
typedef struct mutex {
    int lock;  // 锁标志
} mutex_t;


// 上下文结构体
typedef struct {
    uint32_t cpsr; // 控制寄存器
    uint32_t pc; // 程序计数器
    uint32_t r0; // 寄存器 R0
    uint32_t r1; // 寄存器 R1
    uint32_t r2; // 寄存器 R2
    uint32_t r3; // 寄存器 R3
    uint32_t r4; // 寄存器 R4
    uint32_t r5; // 寄存器 R5
    uint32_t r6; // 寄存器 R6
    uint32_t r7; // 寄存器 R7
    uint32_t r8; // 寄存器 R8
    uint32_t r9; // 寄存器 R9
    uint32_t r10; // 寄存器 R10
    uint32_t r11; // 寄存器 R11
    uint32_t r12; // 寄存器 R12
    uint32_t sp; // 栈指针
    uint32_t lr; // 链接寄存器
} context_t;

void tcb_init(tcb_t *tcb, int priority, int stack_size);
void tcb_destroy(tcb_t *tcb);
tcb_t *get_current_task(void);
tcb_t *get_local_run_queue_head(int cpu_id);
void add_to_global_run_queue(tcb_t *tcb);
tcb_t *take_from_global_run_queue(void);
void add_to_local_run_queue(tcb_t *tcb, int cpu_id);
tcb_t *take_from_local_run_queue(int cpu_id);
void move_current_task_to_global_run_queue(void);
void switch_to_next_task(void);
void task_init(tcb_t *tcb);
tcb_t *create_task(int priority, int stack_size);
void destroy_task(tcb_t *tcb);
void send_message(tcb_t *dest_tcb, int type, void *data);
void receive_message(int type, void *data);
message_queue_t *message_queue_create(int capacity);
void message_queue_destroy(message_queue_t *mq);
int message_queue_push(message_queue_t *mq, message_t *message);
int message_queue_pop(message_queue_t *mq, message_t *message);
int message_queue_push(message_queue_t *mq, message_t *message);
void context_init(context_t *context, void (*entry)(void), void *stack_ptr);
void wait_for_interrupt(void);
int get_cpu_id(void);
void *get_stack_pointer(void);
void set_stack_pointer(void *sp);
void add_interrupt_handler(int irq, void (*handler)(void), int priority);
void enter_critical(void);
void leave_critical(void);
void task_exit(void);
void task_entry(void);
#endif
