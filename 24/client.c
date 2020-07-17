#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#include <queue/queue.h>
/* 定义一个接收队列 */
Queue rec_queue;
#define QUEUE_LEN 4095

#define UDP 0   //0:tcp 1:udp

#if UDP == 1
#include "netinet/udp.h"
#else
#include "netinet/tcp.h"
#endif

/* tcp client */
//struct sockaddr       *addr;   //client socket
struct sockaddr_in    *saddr;  //server socket
/* send buf /recv buf */
#define send_len 4095
#define recv_len 4095
const char send_buf[send_len] = "hellow server!\n";
char       recv_buf[recv_len] =  {0};
/* file operate functions */
char       write_buf[1] = {0};
int recv_file(int fd, void * buf, size_t n,
			 int flag, __SOCKADDR_ARG addr,
			 socklen_t * addr_len,void* arg)
{
    char rec;
    int file_desc;
    int ret;
    char *val;

    /* 打开文件 */
    file_desc = open((char*)arg,O_RDWR|O_CREAT);
    do{
        rec = recvfrom(fd,buf,n,flag,addr,addr_len);
        //读多少写多少
        //if(rec > 0)
        //    write(file_desc,buf,rec);
        /* 入队 */
        if(rec > 0)
        {
            val = (char*)buf;
            ret = In_Queue(&rec_queue,*val);
        }
        if( (ret == OKAY) || (rec <= 0) )
        {
            for(int i=0;i<rec_queue.count;i++)    //一次性取出队列数据写入
            {
                char wr = Out_Queue(&rec_queue);
                write(file_desc,&wr,1);
            }  
        }
    }while(rec > 0);
    close(file_desc);
}




int main(int argc,char** argv)
{
    int cfd;
    int ret;
    int flags = 0;
    int fd;

    /* 接收队列初始化 */
    Creat_Queue(&rec_queue,128);

    //socklen_t c_len = sizeof(struct sockaddr);
    socklen_t s_len = sizeof(struct sockaddr_in);
    saddr  =  (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    /* 创建套接字 */
    cfd = socket(AF_INET,SOCK_STREAM,0);
    /* 设置连接属性 */
    saddr->sin_family = AF_INET;
    inet_aton("192.168.0.128",&saddr->sin_addr);//本地IP
    //saddr->sin_addr.s_addr = inet_addr("127.0.0.1");//本地IP
    saddr->sin_port = htons(1999);//监听端口

    /* 连接服务端 */
    ret = connect(cfd,(struct sockaddr*)saddr,s_len);
    if(ret == -1)
    {
        printf("socket connect failed!\r\n");
        close(cfd);
        exit(-1);
    }
    /* 接收文件 */
    recv_file(cfd,write_buf,1,flags,(struct sockaddr*)saddr,&s_len,argv[1]);

    return 0;
}

