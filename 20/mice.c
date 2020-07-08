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

void main(int argc,char** argv)
{
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
    mice_fd = open(argv[1],O_RDONLY);

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