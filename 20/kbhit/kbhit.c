#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/unistd.h"
#include "sys/stat.h"
#include "sys/syscall.h"
#include "sys/time.h"
#include "string.h"
#include "linux/ioctl.h"
#include "linux/mman.h"
#include "linux/input.h"
#include "linux/input-event-codes.h"
#include "poll.h"
#include "signal.h"
#include "pthread.h"
#include "kbhit.h"

kbhit my_key={
    .key_val =0,
    .flag    =0,
};

void Task_1(void *arg)
{
    /*
    char key;
    printf("Task_1 is running\r\n");
    
    while(1)
    {
        key = getchar();
        if(key == 'a')
        {
            pthread_exit(NULL);
            break;
        }
    }
    */
    /* file desc */
    int key_fd;
    
    /*
     * 键盘事件,event1
     * 鼠标事件,mouse0
     */

    /* return value */
    int ret;
    /* 输入事件 */
    struct input_event input;
    struct pollfd p_fd;
    nfds_t fds = 1;


    key_fd = open(arg,O_RDONLY);
    if(key_fd < 0)
    {
        printf("file open failed!\r\n");
        close(key_fd);
        exit(-1);
    }
    p_fd.fd = key_fd;
    p_fd.events  = EV_KEY;
    p_fd.revents = POLLIN;
    printf("route 1 has been created!\r\n");
    while(1)
    {
        poll(&p_fd,fds,100);
        ret = read(key_fd,&input,sizeof(struct input_event));
        if(ret > 0)
        {
           //printf("type :%d\tvalue :%d\tcode :%d\t\n",input.type,input.value,input.code);
           my_key.key_val = input.value;
           my_key.flag = 1;
        }
    }
    close(key_fd);
}

int _kbhit(int *ret,char* argv)
{
    static pthread_t p_fd[2];
    static int p_id;
    static int lock = 1;

    if(lock)
    {
        p_id = pthread_create(&p_fd[0],NULL,(void*)Task_1,argv);
        printf("main task is running 1!\r\n");
        /* 关闭命令行的回显 */
        system("stty -echo");
        lock=0;
    }

    if(my_key.flag)
    {
        *ret = my_key.key_val;
        my_key.flag = 0;
    }

    if(lock)
        pthread_exit(NULL);
    
    return 0;
}