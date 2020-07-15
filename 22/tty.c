#include "tty.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "termios.h"
#include "sys/ioctl.h"


int open_tty(int fd,char* filename)
{
    fd = open(filename,O_RDWR|O_FSYNC|O_NONBLOCK);//非阻塞打开串口
    if(fd < 0)
    {
        printf("tty open failed!\r\n");
        exit(-1);
    }
    return 0;
}

int close_tty(int fd)
{
    int ret;
    ret = close(fd);
    if(ret < 0)
    {
        printf("tty close failed!\r\n");
        exit(-1);
    }
    return 0;
}

int init_tty(int fd,u32 baudrate,u8 data,u8 stop,u8 parbit)
{
    int ret;
    /*
    ret = ioctl(fd,TCGETS,set);
    if(ret < 0)
    {
        printf("get ttyset failed!\r\n");
        exit(-1);
    }
    */

    set = (struct termios*)malloc(sizeof(struct termios));

    cfsetospeed(set,baudrate);//TX波特率
    cfsetispeed(set,baudrate);//RX波特率
            /* BAUDRATE CONTEXT
            B0      B50     B75     B110    B134    B150
            B200    B300    B600    B1200   B1800   B2400
            B4800   B9600   B19200  B38400  B57600  B115200   B230400
            */
    /* 其他设置 */
    //disable rec irq * 忽略段错误和奇偶校验位 * 
    set->c_iflag |= IGNBRK     | 
                    IGNPAR     ;

    set->c_cflag |= CLOCAL     ;
    switch(data)//数据位
    {
        case 8:set->c_cflag |= CS8;break;
        case 7:set->c_cflag |= CS7;break;
        case 6:set->c_cflag |= CS6;break;
        case 5:set->c_cflag |= CS5;break;
        default:break;
    }
    switch(stop)//停止位
    {
        case 1:set->c_cflag &= (~CSTOPB);break;
        case 2:set->c_cflag |= CSTOPB;break;
        default:break;
    }
    switch(parbit)//奇偶校验位
    {   
        case 'E':set->c_cflag |= PARENB;break;
        case 'N':set->c_cflag &= (~PARENB);break;
        default:break;
    }

    ret = tcsetattr(fd,TCSANOW,set);//设置串口
    if(ret < 0)
    {
        printf("set ttyset failed!\r\n");
        exit(-1);        
    }
    return 0;
}

u8 rec_byte_tty(int fd)
{
    int ret;
    char rec_buf;

    p_fd.fd = fd;
    p_fd.events = POLLIN|POLLRDNORM;
    fds_t = 1;//监测1个设备

    ret = poll(&p_fd,fds_t,100);
    if(ret > 0)
    {
        ret = read(fd,&rec_buf,sizeof(char));
        if(ret > 0)
        {
            //printf("%c\r\n",rec_buf);
            return rec_buf;
        }
    }
    return 0;
}

int rtx_byte_tty(int fd,char rtx_buf)
{
    int ret;

    ret = write(fd,&rtx_buf,sizeof(char));
    if(ret < 0)
    {
        printf("rtx failed!\r\n");
        exit(-1);
    }
    return 0;
}