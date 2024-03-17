# 协程


## 1. 协程的优点
> 异步的执行效率，同步的编程方式.

## 2. 协程的原语操作
> `resume`    ：让出当前操作
> 
> `yield`     ：恢复当前操作

## 3. 协程的切换

### 3.1协程的三种切换方式
- `setjmp` / `longjmp`
    
    > context保存当前协程的寄存器，加载另一个协程的相关寄存器.
    > 
    > C语言的标准库，支持跨平台.
    >
    > 学习一下 `<setjmp.h>` 头文件的函数用法，进行上下文切换.
    >
    > 调用例子放在：src/setjmp.c
- `ucontext`
    
    > linux 内核提供: 上下文环境
    >
    > 学习一下 `<ucontext.h>` 头文件的函数用法，进行上下文切换.
    >
    >  调用例子放在：src/Ucontext.c
- `asm code`
    
    > 汇编代码
    >
    > 参考资料：
    >
    >  doc/X86-64寄存器和栈帧.pdf
    > 优缺点：
    >
    >  优点是性能较高，缺点是跨平台较弱
    > 步骤：
    >  1、定义一个数据结构用于保存协程的上下文信息，例如寄存器状态、栈指针等.
    >
    >  2、在每个协程执行之前，保存当前协程的上下文到该数据结构中，并将其作为参数传递给协程调度器.
    >
    >  3、协程调度器负责在不同的协程之间进行切换.它会根据调度算法选择下一个要执行的协程，并将其上下文恢复到对应寄存器和栈中.
    >
    >  4、使用汇编指令来保存和恢复寄存器状态，以及修改堆栈指针.这包括了 `push` 和 `pop` 指令用于保存和恢复寄存器值，以及 `call` 指令来跳转到新的函数执行.

## 3. 协程的 `struct coroutine` 定义
```c
typedef enum _wly_co_status{
    CO_NEW,         // 新建
    CO_READY,       // 就绪
    CO_WAIT,        // 等待
    CO_SLEEP,       // 超时
    CO_EXIT        // 退出
}wly_co_status_t;

struct coroutine {
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
}
```
## 4. 协程的 调度器（`struct scheduler` ）定义

```c
struct scheduler
{
    struct coroutine *cur_co;           //当前运行的协程Id
    
    queue_head(coroutine) ready_h;      // 就绪队列     头指针
    rbtree_head(coroutine) wait_r;      // 等待队列     头指针
    rbtree_head(coroutine) sleep_r;     // 超时队列     头指针
    queue_head(coroutine) exit_H;       // 退出队列     头指针
};
```

## 5. 协程的调度策略定义

> 调度策略：
>
> 1、io 密集型		把 `wait` 放到最前面，把 `sleep` 放到后面，最后执行 `ready`
>
> 2、计算密集型	把 `ready` 放到最前面

```c
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
```



## 6. 与 posix api 兼容问题

> 使用 `hook` 重新实现socket API函数：`connect`、`send`、`recv` .
>
> 头文件：
>
> windows 平台：`<windows.h>`
>
> linux 平台：`<dlfcn.h>`
