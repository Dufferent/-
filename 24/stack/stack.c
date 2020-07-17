#include "stack.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"


Status Creat_Stack(Stack *s,unsigned int deep)
{
    s->space = (int *)malloc(sizeof(int)*deep);
    for(int i=0;i<deep;i++)
    {
        s->space[i] = 0;
    }
    s->count = 1;
    s->deep  = deep;
    /* 让栈指针指向栈底 */
    s->stack_pt = s->space;
    printf("Stack init success!\r\n");
    return 0;
}

Status Push_Stack(Stack *s,STACK_TYPE val)
{   
    if(s->count != s->deep)
    {
        *(s->stack_pt) = val;
        s->stack_pt++;
        s->count++;
    }
    else if(s->count == s->deep)//单独处理
    {
        *(s->stack_pt) = val;
    }
    else
    {
        printf("Stack is fulled!\r\n");
        return OVERFLOW;
    }
    return OKAY;
}

STACK_TYPE Jump_Stack(Stack *s)
{
    if(s->count > 1)
    {
        STACK_TYPE jump;
        jump = *(s->stack_pt);  
        *(s->stack_pt) = 0;//清除栈内数据
        s->stack_pt--;     //栈指针往栈底方向后退一个单元 
        s->count--;        //栈内计数器递减 
        return jump;
    }
    else if(s->count == 1)//单独处理
    {
        STACK_TYPE jump = *(s->stack_pt);
        *(s->stack_pt) = 0;//清除栈内数据
        return jump;
    }
    else
    {
        printf("Stack is empty!\r\n");
        return INCREABLE;
    }
}