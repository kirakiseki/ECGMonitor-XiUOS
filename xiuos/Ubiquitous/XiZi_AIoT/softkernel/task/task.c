
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
* @file:    task.c
* @brief:   file task.c
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2023/05/18
*
*/

#include <task.h>


// 全局运行队列和本地运行队列
tcb_t *global_run_queue_head = NULL;
tcb_t *global_run_queue_tail = NULL;
tcb_t *local_run_queue_head[8] = {NULL};
tcb_t *local_run_queue_tail[8] = {NULL};

// 当前任务的指针
volatile tcb_t *current_task = NULL;


/*******************************************************************************
* 函 数 名: tcb_init
* 功能描述: 初始化任务控制块
* 形    参: tcb:任务控制块指针,priority:任务优先级,stack_size:栈大小
* 返 回 值: 无
*******************************************************************************/
void tcb_init(tcb_t *tcb, int priority, int stack_size) {
    tcb->priority = priority;
    tcb->state = TASK_CREATED;
    tcb->message_queue = NULL;
    tcb->stack = (uint8_t*)malloc(stack_size);
    tcb->stack_size = stack_size;
    tcb->stack_ptr = tcb->stack + stack_size - sizeof(context_t);
    // 将任务的上下文保存在栈中
    context_t *context = (context_t*)tcb->stack_ptr;
    context_init(context, (void*)task_entry, tcb->stack_ptr);
}

/*******************************************************************************
* 函 数 名: tcb_destroy
* 功能描述: 销毁任务控制块
* 形    参: tcb:任务控制块指针
* 返 回 值: 无
*******************************************************************************/
void tcb_destroy(tcb_t *tcb) {
    free(tcb->stack_bottom);
    tcb->stack_ptr = NULL;
    tcb->stack_bottom = NULL;
    tcb->stack_size = 0;
    tcb->state = TASK_BLOCKED;
    tcb->priority = 0;
    tcb->next = NULL;
    if (tcb->message_queue != NULL) {
        message_queue_destroy(tcb->message_queue);
        tcb->message_queue = NULL;
    }
}

/*******************************************************************************
* 函 数 名: get_current_task
* 功能描述: 获取当前任务的指针
* 形    参: 无
* 返 回 值: 任务控制块指针
*******************************************************************************/
tcb_t *get_current_task() {
    return (tcb_t*)current_task;
}


/*******************************************************************************
* 函 数 名: get_local_run_queue_head
* 功能描述: 获取本地运行队列的头指针
* 形    参: cpu_id:当前cpu的id
* 返 回 值: 任务控制块指针
*******************************************************************************/
tcb_t *get_local_run_queue_head(int cpu_id) {
    return local_run_queue_head[cpu_id];
}


/*******************************************************************************
* 函 数 名: add_to_global_run_queue
* 功能描述: 将任务添加到全局运行队列中
* 形    参: tcb:任务控制块指针
* 返 回 值: 无
*******************************************************************************/
void add_to_global_run_queue(tcb_t *tcb) {
    // 将任务添加到全局运行队列的尾部
    if (global_run_queue_head == NULL) {
        global_run_queue_head = tcb;
        global_run_queue_tail = tcb;
    } else {
        global_run_queue_tail->next = tcb;
        global_run_queue_tail = tcb;
    }
}


/*******************************************************************************
* 函 数 名: take_from_global_run_queue
* 功能描述: 从全局运行队列中取出一个任务
* 形    参: 无
* 返 回 值: tcb:任务控制块指针
*******************************************************************************/
tcb_t *take_from_global_run_queue() {
    // 从全局运行队列的头部取出一个任务
    if (global_run_queue_head == NULL) {
        return NULL;
    } else {
        tcb_t *tcb = global_run_queue_head;
        global_run_queue_head = tcb->next;
        if (global_run_queue_head == NULL) {
            global_run_queue_tail = NULL;
        }
        return tcb;
    }
}


/*******************************************************************************
* 函 数 名: add_to_local_run_queue
* 功能描述: 将任务添加到本地运行队列中
* 形    参: tcb:任务控制块指针,cpu_id:任务绑定的CPU id
* 返 回 值: 无
*******************************************************************************/
void add_to_local_run_queue(tcb_t *tcb, int cpu_id) {
    // 将任务添加到本地运行队列的尾部
    if (local_run_queue_head[cpu_id] == NULL) {
        local_run_queue_head[cpu_id] = tcb;
        local_run_queue_tail[cpu_id] = tcb;
    } else {
        local_run_queue_tail[cpu_id]->next = tcb;
        local_run_queue_tail[cpu_id] = tcb;
    }
}


/*******************************************************************************
* 函 数 名: take_from_local_run_queue
* 功能描述: 从本地运行队列中取出一个任务
* 形    参: cpu_id:CPU id
* 返 回 值: 无
*******************************************************************************/
tcb_t *take_from_local_run_queue(int cpu_id) {
    // 从本地运行队列的头部取出一个任务
    if (local_run_queue_head[cpu_id] == NULL) {
        return NULL;
    } else {
        tcb_t *tcb = local_run_queue_head[cpu_id];
        local_run_queue_head[cpu_id] = tcb->next;
        if (local_run_queue_head[cpu_id] == NULL) {
            local_run_queue_tail[cpu_id] = NULL;
        }
        return tcb;
    }
}


/*******************************************************************************
* 函 数 名: move_current_task_to_global_run_queue
* 功能描述: 将当前任务从本地运行队列中取出,并添加到全局运行队列中
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void move_current_task_to_global_run_queue() {
    int cpu_id = get_cpu_id();
    tcb_t *tcb = (tcb_t*)current_task;
    tcb->state = TASK_READY;
    add_to_global_run_queue(tcb);
    local_run_queue_head[cpu_id] = NULL;
    local_run_queue_tail[cpu_id] = NULL;
    current_task = NULL;
}


/*******************************************************************************
* 函 数 名: switch_to_next_task
* 功能描述: 切换到下一个任务
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void switch_to_next_task() {
    int cpu_id = get_cpu_id();
    tcb_t *current_tcb = (tcb_t*)current_task;
    tcb_t *next_tcb = take_from_local_run_queue(cpu_id);
    if (next_tcb == NULL) {
        next_tcb = take_from_global_run_queue();
    }
    if (next_tcb == NULL) {
        // 如果没有可运行的任务，则将当前任务继续运行
        return;
    }
    // 保存当前任务的上下文
    if (current_tcb != NULL) {
        current_tcb->stack_ptr = get_stack_pointer();
    }
    // 切换到下一个任务的上下文
    current_task = next_tcb;
    set_stack_pointer(next_tcb->stack_ptr);
    // 如果切换到的任务是新的任务，需要执行任务初始化函数
    if (current_tcb != next_tcb) {
        next_tcb->state = TASK_RUNNING;
        task_init(next_tcb);
    }
}


/*******************************************************************************
* 函 数 名: task_init
* 功能描述: 任务初始化函数
* 形    参: tcb:任务控制块指针
* 返 回 值: 无
*******************************************************************************/
void task_init(tcb_t *tcb) {
    // 创建消息队列
    message_queue_t mq = (message_queue_t)malloc(sizeof(message_queue_t));
    mq->buffer = (message_t)malloc(sizeof(message_t) * 16);
    mq->capacity = 16;
    mq->head = 0;
    mq->tail = 0;
    mq->count = 0;
    tcb->message_queue = mq;
}


/*******************************************************************************
* 函 数 名: create_task
* 功能描述: 创建任务
* 形    参: priority:任务优先级,stack_size:栈大小
* 返 回 值: 无
*******************************************************************************/
tcb_t *create_task(int priority, int stack_size) {
    tcb_t *tcb = (tcb_t*)malloc(sizeof(tcb_t));
    tcb_init(tcb, priority, stack_size);
    add_to_global_run_queue(tcb);
    return tcb;
}


/*******************************************************************************
* 函 数 名: destroy_task
* 功能描述: 销毁任务
* 形    参: tcb:任务控制块指针
* 返 回 值: 无
*******************************************************************************/
void destroy_task(tcb_t *tcb) {
    tcb_destroy(tcb);
    free(tcb);
}


/*******************************************************************************
* 函 数 名: send_message
* 功能描述: 发送消息
* 形    参: dest_tcb:目标任务的控制块指针,type:消息类型,data:消息的具体内容
* 返 回 值: 无
*******************************************************************************/
void send_message(tcb_t *dest_tcb, int type, void *data) {
    message_t message;
    message.type = type;
    message.data = data;
    if (dest_tcb->message_queue == NULL) {
        // 如果目标任务没有消息队列，则创建一个消息队列
        dest_tcb->message_queue = message_queue_create(16);
    }
    // 将消息添加到目标任务的消息队列中
    message_queue_push(dest_tcb->message_queue, &message);
}


/*******************************************************************************
* 函 数 名: receive_message
* 功能描述: 接收消息
* 形    参: type:接收的消息类型,data:存储接收到的消息
* 返 回 值: 无
*******************************************************************************/
void receive_message(int type, void *data) {
    tcb_t *current_tcb = (tcb_t*)current_task;
    message_t message;
    // 从当前任务的消息队列中取出消息
    while (message_queue_pop(current_tcb->message_queue, &message) != 0) {
        if (message.type == type) {
            // 如果消息的类型匹配，则返回消息数据
            memcpy(data, message.data, sizeof(void*));
            return;
        }
    }
    // 如果没有匹配的消息，则当前任务被阻塞，直到有匹配的消息为止
    current_tcb->state = TASK_BLOCKED;
    while (message_queue_pop(current_tcb->message_queue, &message) != 0) {
        if (message.type == type) {
            // 如果有匹配的消息，则返回消息数据，并将当前任务状态设置为就绪
            memcpy(data, message.data, sizeof(void*));
            current_tcb->state = TASK_READY;
            return;
        }
    }
    // 如果还是没有匹配的消息，则当前任务一直被阻塞
    while (1) {
        wait_for_interrupt();
    }
}


/*******************************************************************************
* 函 数 名: message_queue_create
* 功能描述: 创建消息队列
* 形    参: capacity:消息队列的容量
* 返 回 值: 生成的消息队列的指针
*******************************************************************************/
message_queue_t *message_queue_create(int capacity) {
    message_queue_t *mq = (message_queue_t*)malloc(sizeof(message_queue_t));
    mq->buffer = (message_t*)malloc(sizeof(message_t) * capacity);
    mq->capacity = capacity;
    mq->head = 0;
    mq->tail = 0;
    mq->count = 0;
    return mq;
}


/*******************************************************************************
* 函 数 名: message_queue_destroy
* 功能描述: 销毁消息队列
* 形    参: mq:消息队列的指针
* 返 回 值: 无
*******************************************************************************/
void message_queue_destroy(message_queue_t *mq) {
    free(mq->buffer);
    free(mq);
}


/*******************************************************************************
* 函 数 名: message_queue_push
* 功能描述: 将消息添加到消息队列中
* 形    参: message_queue_t:消息队列的指针,message_t:存储的消息
* 返 回 值: 0:成功,-1:失败
*******************************************************************************/
int message_queue_push(message_queue_t *mq, message_t *message) {
    if (mq->count >= mq->capacity) {
        return -1;  // 队列已满
    }
    mq->buffer[mq->tail] = *message;
    mq->tail = (mq->tail + 1) % mq->capacity;
    mq->count++;
    return 0;
}


/*******************************************************************************
* 函 数 名: message_queue_pop
* 功能描述: 从消息队列中取出消息
* 形    参: message_queue_t:消息队列的指针,message_t:存储的消息
* 返 回 值: 0:成功,-1:失败
*******************************************************************************/
int message_queue_pop(message_queue_t *mq, message_t *message) {
    if (mq->count <= 0) {
        return -1;  // 队列已空
    }
    *message = mq->buffer[mq->head];
    mq->head = (mq->head + 1) % mq->capacity;
    mq->count--;
    return 0;
}


/*******************************************************************************
* 函 数 名: context_init
* 功能描述: 初始化上下文
* 形    参: message_queue_t:消息队列的指针,message_t:存储的消息
* 返 回 值: 0:成功,-1:失败
*******************************************************************************/
void context_init(context_t *context, void (*entry)(void), void *stack_ptr) {
    context->cpsr = 0x10;  // 设置 CPSR 的初始值
    context->pc = (uint32_t)entry;  // 设置 PC 的初始值为任务入口地址
    context->r0 = (uint32_t)stack_ptr;  // 设置 R0 的初始值为栈指针
    context->r1 = 0;  // 设置 R1 的初始值为 0
    context->r2 = 0;  // 设置 R2 的初始值为 0
    context->r3 = 0;  // 设置 R3 的初始值为 0
    context->r4 = 0;  // 设置 R4 的初始值为 0
    context->r5 = 0;  // 设置 R5 的初始值为 0
    context->r6 = 0;  // 设置 R6 的初始值为 0
    context->r7 = 0;  // 设置 R7 的初始值为 0
    context->r8 = 0;  // 设置 R8 的初始值为 0
    context->r9 = 0;  // 设置 R9 的初始值为 0
    context->r10 = 0;  // 设置 R10 的初始值为 0
    context->r11 = 0;  // 设置 R11 的初始值为 0
    context->r12 = 0;  // 设置 R12 的初始值为 0
    context->sp = (uint32_t)stack_ptr;  // 设置 SP 的初始值为栈指针
    context->lr = (uint32_t)task_exit;  // 设置 LR 的初始值为线程退出函数的地址
}

/*******************************************************************************
* 函 数 名: wait_for_interrupt
* 功能描述: 等待中断
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void wait_for_interrupt() {
    // 在实现中，可以使用 WFI 指令等待中断
    __asm__ volatile("wfi");
}

/*******************************************************************************
* 函 数 名: get_cpu_id
* 功能描述: 获取CPU id
* 形    参: 无
* 返 回 值: 获取的cpu id
*******************************************************************************/
int get_cpu_id() {
    uint32_t mpidr;
    __asm__ volatile("mrc p15, 0, %0, c0, c0, 5" : "=r"(mpidr));
    return mpidr & 0xff;
}


/*******************************************************************************
* 函 数 名: get_stack_pointer
* 功能描述: 获取栈指针
* 形    参: 无
* 返 回 值: 获取到的栈指针
*******************************************************************************/
void *get_stack_pointer() {
    // 在实现中，可以使用 ARM 的 SP 寄存器获取栈指针
    void *sp;
    __asm__ volatile("mov %0, sp" : "=r"(sp));
    return sp;
}


/*******************************************************************************
* 函 数 名: set_stack_pointer
* 功能描述: 设置栈指针
* 形    参: 设置的栈指针
* 返 回 值: 无
*******************************************************************************/
void set_stack_pointer(void *sp) {
    // 在实现中，可以使用 ARM 的 SP 寄存器设置栈指针
    __asm__ volatile("mov sp, %0" : : "r"(sp));
}

/*******************************************************************************
* 函 数 名: add_interrupt_handler
* 功能描述: 添加中断处理函数
* 形    参: irq:中断号,handler:中断句柄,priority:中断优先级
* 返 回 值: 无
*******************************************************************************/
void add_interrupt_handler(int irq, void (*handler)(void), int priority) {
    uint32_t *vector_table = (uint32_t*)0x0;
    
    // 将中断处理函数写入中断向量表中
    vector_table[irq + 16] = (uint32_t)handler;
    
    // 设置中断优先级
    uint8_t priority_shift = 8 - __NVIC_PRIO_BITS;
    uint32_t priority_mask = 0xff << priority_shift;
    uint32_t priority_value = (priority << priority_shift) & priority_mask;
    uint32_t *priority_reg = (uint32_t*)0xE000E400;
    int index = irq / 4;
    int offset = irq % 4;
    priority_reg[index] &= ~(0xff << (offset * 8));
    priority_reg[index] |= (priority_value << (offset * 8));
    
    // 使能中断
    uint32_t *enable_reg = (uint32_t*)0xE000E100;
    enable_reg[irq / 32] |= 1 << (irq % 32);
}


/*******************************************************************************
* 函 数 名: enter_critical
* 功能描述: 进入临界区
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void enter_critical() {
    // 在实现中，可以使用 ARM 的 CPSR 寄存器进入临界区
    __asm__ volatile("cpsid i");
}

/*******************************************************************************
* 函 数 名: leave_critical
* 功能描述: 离开临界区
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void leave_critical() {
    // 在实现中，可以使用 ARM 的 CPSR 寄存器离开临界区
    __asm__ volatile("cpsie i");
}



/*******************************************************************************
* 函 数 名: task_exit
* 功能描述: 任务退出函数
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void task_exit() {
    // 切换到下一个任务
    switch_to_next_task();
    // 在实现中，可以使用 ARM 的 WFE 指令等待下一个中断
    __asm__ volatile("wfe");
}


/*******************************************************************************
* 函 数 名: task_entry
* 功能描述: 任务入口函数
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void task_entry() {
    tcb_t *current_tcb = (tcb_t*)current_task;
    current_tcb->state = TASK_RUNNING;
    task_init(current_tcb);
    while (1) {
        switch_to_next_task();
    }
}
