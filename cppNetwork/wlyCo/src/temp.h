



typedef enum _co_status{
    CO_NEW,         // 新建
    CO_READY,       // 就绪
    CO_WAIT,        // 等待
    CO_SLEEP,       // 超时
    CO_EXIT,        // 退出
}co_status_t;

typedef struct _coroutine_s {
    int coid;							// 协程 Id
    struct context ctx;					// 寄存器、栈 上下文， 用于协程的切换
    void* (*entry)(void *);				// 协程入口函数
    void *arg;							// 入口函数的参数
    void *stack;                        // 独立栈（隔离性好，内存利用率不高），共享栈(内存利用率高，隔离性不好)      这里使用独立栈
    size_t size;                        // 栈的大小
    co_status_t status;                 // 栈的状态

    queue_node(coroutine_t) ready_q;      // 就绪队列     使用队列结构，先进先出特性
    rbtree_node(coroutine_t) wait_t;      // 等待队列     使用红黑树，需要根据时长进行排序
    rbtree_node(coroutine_t) sleep_t;     // 超时队列     使用红黑树，需要根据时长进行排序
    queue_node(coroutine_t) exit_q;       // 退出队列     使用队列结构，不需要根据属性进行排序，按照先后顺序即可
}coroutine_t;


typedef struct _scheduler_s
{
    coroutine_t *cur_co;           // 当前运行的协程Id
    
    queue_head(coroutine_t) ready_h;      // 就绪队列     头指针
    rbtree_head(coroutine_t) wait_r;      // 等待队列     头指针
    rbtree_head(coroutine_t) sleep_r;     // 超时队列     头指针
    queue_head(coroutine_t) exit_H;       // 退出队列     头指针
}scheduler_t;


void schedul(scheduler_t* sched) {

    coroutine_t *co = NULL;

    // 将超时队列中的协程 放入就绪队列
    while ((co = get_next_node(sched->sleep_r)) != NULL) {

        push_ready_queue(co);
        co->status = CO_READY;
    }

    // 将等待队列中的协程 加入就绪队列
    while ((co = get_next_node(sched->wait_r)) != NULL)  {

        push_ready_queue(co);
        co->status = CO_READY;
    }

    // 将就绪队列中的协程 执行
    while ((co = get_next_node(sched->ready_h)) != NULL)  {

        resume(co);     // 在执行的函数中调用：co->status = CO_READY;
    }
    
    // 将退出队列中的协程 销毁
    while ((co = get_next_node(sched->exit_H)) != NULL)  {

        destory(co);
    }
}