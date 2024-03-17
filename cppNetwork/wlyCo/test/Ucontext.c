#include <stdio.h>
#include <ucontext.h>


/**
 * 
 *      #include <ucontext.h> 是C语言中的头文件，它包含了一些用于协程和上下文切换的函数和数据类型。其中最常用的是 ucontext_t 结构体，它用于保存线程或协程的上下文信息。
 *      ucontext.h 中定义了一些函数和宏来操作 ucontext_t 结构体，比如：
 *      getcontext()：获取当前上下文信息，并保存到指定的 ucontext_t 结构体中。
 *      setcontext()：将指定的 ucontext_t 上下文信息恢复为当前上下文，并开始执行。
 *      makecontext()：创建一个新的上下文，并关联一个函数以及相应的参数，这样可以在该上下文中执行该函数。
 *      swapcontext()：保存当前上下文，并切换到另一个指定的上下文中继续执行。
 * 
*/

ucontext_t ctx[2];
ucontext_t main_ctx;

int count = 0;

void func1()
{
    while (count++ < 100)
    {
        printf("func1 start.\n");
        swapcontext(&ctx[0], &ctx[1]);
        printf("func1 end.\n");
    }
}


void func2()
{
    while (count++ < 100)
    {
        printf("func2 start.\n");
        swapcontext(&ctx[1], &ctx[0]);
        printf("func2 end.\n");
    }
}


int main()
{
    char stack1[2048] = {0};
    char stack2[2048] = {0};

    getcontext(&ctx[0]);
    ctx[0].uc_stack.ss_sp = stack1;
    ctx[0].uc_stack.ss_size = sizeof(stack1);
    ctx[0].uc_link = &main_ctx;
    makecontext(&ctx[0], func1, 0);

    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack2;
    ctx[1].uc_stack.ss_size = sizeof(stack2);
    ctx[1].uc_link = &main_ctx;
    makecontext(&ctx[1], func2, 0);
    
    swapcontext(&main_ctx, &ctx[0]);


    return 0;
}