
#include <stdio.h>
#include <setjmp.h>



//
//  setjmp   ,       longjmp

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