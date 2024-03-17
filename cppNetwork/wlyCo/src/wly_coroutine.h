



#ifndef __WLY_COROUTINE_H__
#define __WLY_COROUTINE_H__


#include <stdio.h>
#include <stdlib.h>

#include "wly_queue.h"
#include "wly_tree.h"


typedef enum _wly_co_status{
    
    CO_NEW,         // 新建
    CO_READY,       // 就绪
    CO_WAIT,        // 等待
    CO_SLEEP,       // 超时
    CO_EXIT        // 退出

} wly_co_status_t;

typedef struct _wly_coroutine_s {
    int coid;							// 协程 Id
    struct context ctx;					// 寄存器、栈 上下文， 用于协程的切换
    void* (*entry)(void *);				// 协程入口函数
    void *arg;							// 入口函数的参数
    void *stack;                        // 独立栈（隔离性好，内存利用率不高），共享栈(内存利用率高，隔离性不好)      这里使用独立栈
    size_t size;                        // 栈的大小
    co_status_t status;                 // 协程的状态

    queue_node(coroutine) ready_q;      // 就绪队列     使用队列结构，先进先出特性
    rbtree_node(coroutine) wait_t;      // 等待队列     使用红黑树，需要根据时长进行排序
    rbtree_node(coroutine) sleep_t;     // 超时队列     使用红黑树，需要根据时长进行排序
    queue_node(coroutine) exit_q;       // 退出队列     使用队列结构，不需要根据属性进行排序，按照先后顺序即可
}wly_coroutine_t;

typedef struct _wly_scheduler_s {

    struct coroutine *cur_co;           //当前运行的协程Id
    
    queue_head(coroutine) ready_h;      // 就绪队列     头指针
    rbtree_head(coroutine) wait_r;      // 等待队列     头指针
    rbtree_head(coroutine) sleep_r;     // 超时队列     头指针
    queue_head(coroutine) exit_H;       // 退出队列     头指针

}wly_scheduler_t;









#denif