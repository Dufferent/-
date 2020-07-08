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

struct kbhit{
    int key_val;
};
struct kbhit my_key;


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
    while(my_key.key_val != 30)
    {
        poll(&p_fd,fds,100);
        ret = read(key_fd,&input,sizeof(struct input_event));
        if(ret > 0)
        {
           printf("type :%d\tvalue :%d\tcode :%d\t\n",input.type,input.value,input.code);
           my_key.key_val = input.value;
        }
    }
    close(key_fd);
}

void Task_2(void *arg)
{
    /*
    char key;
    printf("Task_2 is running\r\n");
    
    while(1)
    {
        key = getchar();
        if(key == 'b')
        {
            pthread_exit(NULL);
            break;
        }
    }
    */
   int mice_fd;
    /*
     * 键盘事件,event1
     * 鼠标事件,mouse0
     */

    /* 输入事件 */
    //struct input_event input;
    struct pollfd p_fd;
    nfds_t fds = 1;

    /* return value */
    int ret;
    mice_fd = open(arg,O_RDONLY);

    p_fd.fd = mice_fd;
    p_fd.events  = EV_ABS;
    p_fd.revents = POLLIN;

    /* 输入事件 */
    char buf[6] = {0};
    while(1)
    {
        poll(&p_fd,fds,100);
        ret = read(mice_fd,buf,6);
        if(ret > 0)
        {
           printf("type :%d\tX :%d\tY :%d\tZ :%d\n",(buf[0]&0x07),buf[1],buf[2],buf[3]);
           //printf("type :%d\tvalue :%d\tcode :%d\t\n",input.type,input.value,input.code);
        }
    }
    close(mice_fd);
}

int main(int argc,char** argv)
{
    pthread_t p_fd[2];
    int p_id;

    p_id = pthread_create(&p_fd[0],NULL,(void*)Task_1,argv[1]);
    printf("main task is running 1!\r\n");
    p_id = pthread_create(&p_fd[1],NULL,(void*)Task_2,argv[2]);
    printf("main task is running 2!\r\n");
    pthread_exit(NULL);

    return 0;
}