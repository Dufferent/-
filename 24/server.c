#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#define UDP 0   //0:tcp 1:udp

#if UDP == 1
#include "netinet/udp.h"
#else
#include "netinet/tcp.h"
#endif

/* send buf /recv buf */
#define send_len 4095
#define recv_len 4095
const char send_buf[send_len] = "hellow client!\n";
char       recv_buf[recv_len] =  {0};
/* server */
struct sockaddr_in *addr;  //server set
struct sockaddr    *caddr; //client get
/* file buf */
char read_buf[1];
/* send file functions */
int send_file(int fd,const void* buf,size_t n,int flag)
{
    int ret;
    ret = send(fd,buf,n,flag);
    if(ret == -1)
    {
        printf("send error!\r\n");
        send_file(fd,buf,n,flag);
        return -1;
    }
    else
    {
        //printf("send success!\r\n");
        return 0;
    }
}



int main(int argc,char* argv[])
{
    int sfd;
    int cfd;
    int ret;
    int flags = 0;
    int fd;
    char read_size = 0;
    socklen_t s_len = sizeof(struct sockaddr_in);;
    socklen_t c_len = sizeof(struct sockaddr);
    /* 创建一个新的套接字 */
    sfd = socket(AF_INET,SOCK_STREAM,0);//tcp
  //sdf = socket(AF_INET,SOCK_DGRAM,0);//udp
    if(sfd < 0 )
    {
        printf("socket request failed!\r\n");
        close(sfd);
        exit(-1);
    }

    /* 分配空间 */
    addr  = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    caddr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    /* 绑定IP和端口 */
    //1.清空地址结构体
    memset(addr,0,sizeof(struct sockaddr_in));
    //2.设置地址结构体
    addr->sin_family = AF_INET;
    inet_aton("192.168.0.128",&addr->sin_addr);//本地IP
    //addr->sin_addr.s_addr = inet_addr("127.0.0.1");//本地IP
    addr->sin_port = htons(1999);//监听端口
    

    ret = bind(sfd,(struct sockaddr*)addr,s_len);
    if(ret < 0)
    {
        printf("socket bind failed!\r\n");
        close(sfd);
        exit(-1);
    }

    //3.开始监听
    ret = listen(sfd,1);//一个套接字只监听一个client
    if(ret < 0)
    {
        printf("socket linsten failed!\r\n");
        close(sfd);
        exit(-1);
    }

    
    while(1)
    {
        //4.等待连接
        cfd = accept(sfd,caddr,&c_len);//阻塞，连接成功后得到client的信息结构体caddr
        
        //5.发送文件给客户端
        /* 打开文件 */
        fd = open(argv[1],O_RDWR|O_CREAT);
        if(fd < 0 )
        {
            printf("open file failed!\r\n");
            close(cfd);
            close(sfd);
            exit(-1);
        }

        do{
            read_size = read(fd,read_buf,1);//一次读一个字节
            /* 读多少发多少 */
            if(read_size > 0)
            {
                ret = send_file(cfd,read_buf,1,flags);
                if(ret < 0)
                {
                    printf("error for send byte!\r\n");
                    close(cfd);
                    close(sfd);
                    exit(-1);
                }
            }
            usleep(10*1000);//30ms的间隔，等待接收完成
        }while(read_size > 0);
        printf("send all!\r\n");
        close(cfd);
    }
    close(cfd);
    close(sfd);
    return 0;
}