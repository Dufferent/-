#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"
#include "iostream"
#include "string.h"
//网络编程
#include "sys/socket.h"
#include "netinet/tcp.h"
#include "netinet/in.h"
#include "netinet/udp.h"
#include "arpa/inet.h"


using namespace std;
#define BUF_LEN 1024
char R_BUF[BUF_LEN] = {0};
char S_BUF[BUF_LEN] = {0};

//文件互传 客户端
int main(int argc,char **argv)
{
    int ret;
    int sfd;
    int cfd;
    int fd;
    int rec;
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    socklen_t slen;
    socklen_t clen;
    int flags = 0;
    char cmdbuf[64] = {0};
    if(argc < 6)
    {
        printf("cmd error!\ntry like:scpc -s|-g [host] [port] [filedir] [local-dir]\n");
        cout<<"\t\t[-s|-g]:-s:send file in [local-dir] to host:port's filedir\r\n"<<endl;
        cout<<"\t\t        -g:get a file save to [local-dir] in host:port's filedir\r\n"<<endl;
        exit(-1);
    }
    //初始化客户端套接字
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    caddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    caddr.sin_family = AF_INET;
    caddr.sin_port = 7441;
    clen = sizeof(struct sockaddr_in);
    //bind
    ret = bind(cfd,(sockaddr*)&caddr,clen);
    if(ret == -1)
    {
        cout<<"error bind!"<<endl;
        exit(-1);
    }
    //填充服务器套接字信息
    saddr.sin_addr.s_addr = inet_addr(argv[2]);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[3]));
    //链接服务器
    slen = sizeof(struct sockaddr_in);
    ret = connect(cfd,(sockaddr*)&saddr,slen);
    if(ret == -1)
    {
        cout<<"error connect!"<<endl;
        exit(-1);
    }
    //处理cmd
    if( !strcmp(argv[1],"-s") )
    {
        sprintf(S_BUF,"%s%c%s%c","-s",'#',argv[4],'#');//报文
        //发送报文
        sendto(cfd,S_BUF,BUF_LEN,0,(sockaddr*)&saddr,slen);
        usleep(1000);//wait recv
        memset(S_BUF,0,BUF_LEN);
        //发送文件本体
        //打开文件
        fd = open(argv[5],O_RDONLY);
        if(fd == -1)
        {
            cout<<"error open file!"<<endl;
            exit(-1);
        }
        do{
            rec = read(fd,S_BUF,BUF_LEN);
            if(rec > 0)
                sendto(cfd,S_BUF,rec,0,(sockaddr*)&saddr,slen);//读多少发多少
            usleep(1000);
        }while(rec > 0);
        close(fd);
    }
    else if( !strcmp(argv[1],"-g") )
    {
        sprintf(S_BUF,"%s%c%s%c","-g",'#',argv[4],'#');//报文
        //发送报文
        sendto(cfd,S_BUF,BUF_LEN,0,(sockaddr*)&saddr,slen);
        usleep(1000);//wait recv
        memset(R_BUF,0,BUF_LEN);
        //打开文件
        fd = open(argv[5],O_RDWR|O_CREAT);
        if(fd == -1)
        {
            cout<<"error open file!"<<endl;
            exit(-1);
        }
        do{
            rec = recvfrom(cfd,R_BUF,BUF_LEN,0,(sockaddr*)&saddr,&slen);
            if(rec > 0)
                write(fd,R_BUF,rec);
            sprintf(cmdbuf,"%s%s","chmod 777 ",argv[5]);
            system(cmdbuf);
        }while(rec > 0);
        close(fd);
    }
    else
        close(fd);
    return 0;
}
