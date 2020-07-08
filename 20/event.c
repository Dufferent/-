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


struct kbhit{
    int key_val;
};
struct kbhit my_key;


void main(int argc,char** argv)
{
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


    key_fd = open(argv[1],O_RDONLY);
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