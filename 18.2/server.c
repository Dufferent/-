#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#define UDP 1   //0:tcp 1:udp

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


int main(void)
{
    int sfd;
    int cfd;
    int ret;
    int flags = 0;
    socklen_t s_len = sizeof(struct sockaddr_in);;
    socklen_t c_len = sizeof(struct sockaddr);
    /* 创建一个新的套接字 */
    //sfd = socket(AF_INET,SOCK_STREAM,0);//tcp
    sfd = socket(AF_INET,SOCK_DGRAM,0);//udp
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
    /*
     * udp只要完成udp-socket
     * 绑定地址：端口
     * 就可以直接使用
     */
    
    while(1)
    {
        ret = recvfrom(sfd,recv_buf,recv_len,flags,caddr,&c_len);
        if(ret < 0)
        {
            printf("recv failed!\r\n");
            close(sfd);
        }
        printf("%s\n",recv_buf);
    }
    close(sfd);
    return 0;
}