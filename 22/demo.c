#include <tty.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv)
{
    int fd;
    char rec;

    open_tty(fd,argv[1]);
    init_tty(fd,115200,8,1,'N');
    /*
    while(1)
    {
        rec = rec_byte_tty(fd);
        if(rec>0)
        {
            printf("rec is:%c\r\n",rec);
        }
        if(rec == 'e')
        {
            printf("Done!\r\n");
            break;
        }
    }
    */
    rtx_byte_tty(fd,'a');

    /* 关闭串口 */
    close_tty(fd);


    return 0;
}
