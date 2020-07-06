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

/* tcp client */
struct sockaddr       *addr;   //client socket
struct sockaddr_in    *saddr;  //server socket
/* send buf /recv buf */
#define send_len 4095
#define recv_len 4095
const char send_buf[send_len] = "hellow server!\n";
char       recv_buf[recv_len] =  {0};


int main(void)
{
    int cfd;
    int ret;
    int flags = 0;
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

    ret = recvfrom(cfd,recv_buf,recv_len,flags,(struct sockaddr*)saddr,&s_len);//接收服务端的信息，block
    if(ret == -1)
    {
        printf("recv msg failed!\r\n");
        close(cfd);
        exit(-1);
    }
    printf("recv msg:%s\n",recv_buf);
    ret = sendto(cfd,
                send_buf,
                send_len,
                flags,
                (struct sockaddr *)saddr,
                s_len);
    if(ret == -1)
    {
        printf("send msg failed!\r\n");
        close(cfd);
        exit(-1);
    }
    return 0;
}

