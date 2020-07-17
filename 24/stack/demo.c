#include "stack.h"
#include "stdio.h"
#include "stdlib.h"

int main(void)
{
    Stack my_stack;
    STACK_TYPE val[10]={1,2,3,4,5,6,7,8,9,10};

    Creat_Stack(&my_stack,10);

    /* 压栈 */
    for(int i=0;i<my_stack.deep;i++)
        Push_Stack(&my_stack,val[i]);

    /* 弹栈 */
    for(int i=0;i<my_stack.deep;i++)
        printf("%3d",Jump_Stack(&my_stack));

    printf("\r\n");
    /* 对比压栈前 */
    for(int i=0;i<my_stack.deep;i++)
        printf("%3d",val[i]);

    printf("\r\n");
    

    return 0;
}