#include "queue.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "string.h"

Status Creat_Queue(Queue *s,unsigned int len)
{
    s->data = (QUEUE_TYPE*)malloc(sizeof(QUEUE_TYPE)*len);
    memset(s->data,0,sizeof(QUEUE_TYPE)*len);
    s->len  = len;
    s->end  = s->data;//尾指针指向队列尾
    s->head = s->data;//头指针指向下一个队列空间
    s->count= 0;
    return 0;
}

Status In_Queue(Queue *s,QUEUE_TYPE val)
{
    if(s->count < s->len)
    {
        *(s->head) = val;//在头指针处添加队列新成员
        s->head++;       //头指针向后移动
        s->count++;      //队列计数器自增
        return 1;
    }
    else if(s->count = s->len)
    {
        *(s->head) = val;//在头指针处添加队列新成员
        return OKAY;
    }
    else
    {
        printf("Queue is fulled!\r\n");
        return OVERFLOW;
    }
}

QUEUE_TYPE Out_Queue(Queue *s)
{
    QUEUE_TYPE ret;
    if(s->end < s->head)
    {
        ret = *(s->end);//先入先出
        s->end++;       //尾指针前移
        return ret;
    }
    else if(s->end == s->head)//头尾重合，空对列
    {
        ret = *(s->end);//先入先出
        return ret;
    }
    else
    {
        printf("No data in the queue!\r\n");
        return INCREABLE;
    }
}