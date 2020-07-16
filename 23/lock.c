#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "pthread.h"

#define PTHREAD_NUMS 2

struct my_pthr{
    pthread_t pth;
    pthread_attr_t pth_attr;
    void *(*pthread_handler)(void *);
    char desc[32];
};
pthread_rwlock_t rwlock;
pthread_rwlockattr_t rwlock_attr;
struct my_pthr pthread[2];

char ts_buf[100]={0};

/* 线程1 */
void task_1(void* arg)
{
    int ret;

    printf("task_1 has been run!\r\n");
    printf("task describe info :%s\r\n",pthread[0].desc);
    while(1)
    {
        ret = pthread_rwlock_wrlock(&rwlock); //写锁
        if(ret < 0)
        {
            printf("写锁上锁失败!\r\n");
            exit(-1);
        }
        sprintf(ts_buf,"%s","task_1 write something here!");
        usleep(1000);                    //五秒这里应该是持续的读阻塞
        ret = pthread_rwlock_unlock(&rwlock); //解锁
        if(ret < 0)
        {
            printf("解锁失败!\r\n");
            exit(-1);
        }
    }
}

/* 线程2 */
void task_2(void* arg)
{
    int ret;

    printf("task_2 has been run!\r\n");
    printf("task describe info :%s\r\n",pthread[1].desc);
    while(1)
    {
        ret = pthread_rwlock_rdlock(&rwlock);
        printf("rec buf:%s\r\n",ts_buf);
        ret = pthread_rwlock_unlock(&rwlock); //解锁
        if(ret < 0)
        {
            printf("解锁失败!\r\n");
            exit(-1);
        }
    }
}




int main(int argc,char** argv)
{
    int ret;
    pthread[0].pthread_handler = (void *)task_1;
    pthread[1].pthread_handler = (void *)task_2;

    /* 初始化读写锁 */
    ret = pthread_rwlock_init(&rwlock,NULL);
    if(ret < 0)
    {
        printf("读写锁申请失败!\r\n");
        exit(-1);
    }


    /* 初始化线程 */
    for(int i=0;i<PTHREAD_NUMS;i++)
    {
        sprintf(pthread[i].desc,"%s%d","task_",i+1);
        ret = pthread_create(&pthread[i].pth,NULL,(void*)pthread[i].pthread_handler,argv[i]);
        if(ret < 0)
        {
            printf("pthread creat failed!\r\n");
            exit(-1);
        }
    }

    pthread_exit(NULL);//创建任务结束
    return 0;
}