#include "stdio.h"
#include "queue.h"

int main(void)
{
    Queue my_queue;
    QUEUE_TYPE val[10]={1,2,3,4,5,6,7,8,9,10};

    Creat_Queue(&my_queue,10);

    for(int i=0;i<10;i++)//入队
        In_Queue(&my_queue,val[i]);

    for(int i=0;i<10;i++)//出队
        printf("%3d",Out_Queue(&my_queue));

    printf("\r\n");

    for(int i=0;i<10;i++)//对比
        printf("%3d",val[i]);

    printf("\r\n");
    return 0;
}