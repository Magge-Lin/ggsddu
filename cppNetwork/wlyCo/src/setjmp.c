
#include <stdio.h>
#include <setjmp.h>

/**
 * 
 *      setjmp.h 是C语言标准库中的一个头文件，它声明了一组函数和宏，用于实现非局部跳转（non-local jumps）的功能。其中最著名的函数是 setjmp 和 longjmp。
 *
 *      setjmp 函数：用于保存当前执行环境，并返回0作为结果。
 *      longjmp 函数：将之前通过 setjmp 保存的执行环境恢复到指定位置，并使得 setjmp 返回一个非零值。
 *
 *      这种非局部跳转机制可以绕过常规的函数调用栈，直接在程序中跳转到任意位置，通常用于异常处理或者线程切换等需要在不同上下文之间进行跳转的场景。
 *
 *      需要注意的是，使用 setjmp.h 中的非局部跳转功能要谨慎，因为它会打破正常的程序流程控制，可能导致代码逻辑混乱或难以理解。
 * 
*/

//
//  setjmp  ,   longjmp

jmp_buf env;

void func(int arg)
{
    printf("func.   \n");
    longjmp(env, ++arg);
    printf("longjmp complete.   \n");
}

int main()
{
    printf("start .   \n");
    int ret = setjmp(env);
    printf("setjmp.   \n");
    if (ret == 0)
    {
        printf("ret == 0\n");
        func(ret);
        
    }
    else if (ret == 1)
    {
        printf("ret == 1\n");

        func(ret);
    }
    else
    {
        printf("ret > 1. \n");
    }
    
    printf("ret = %d\n", ret);


    return 0;
}