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
char RD_BUF[BUF_LEN] = {0};
char WR_BUF[BUF_LEN] = {0};
char process[2][128] = {0};

int myscp(char** argv)
{
    int ret;
    int sfd;
    int cfd;
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    socklen_t slen;
    socklen_t clen;
    int flags = 0;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
    {
        cout<<"error socket!"<<endl;
        exit(-1);
    }
    //初始化服务器套接字信息
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(argv[1]);//输入的第一个命令是local-host
    saddr.sin_port = htons(atoi(argv[2]));     //输入的第二个命令是local-port
    slen = sizeof(struct sockaddr_in);
    //绑定端口
    ret = bind(sfd,(sockaddr*)&saddr,slen);
    if(ret == -1)
    {
        cout<<"error bind!"<<endl;
        exit(-1);
    }
    //监听
    ret = listen(sfd,1);//监听一个端口
    if(ret == -1)
    {
        cout<<"error listen!"<<endl;
        exit(-1);
    }
    //等待链接
    while(1)
    {
        int rec = 0;
        int fd;
        char cmdbuf[64] = {0};
        cfd = accept(sfd,(sockaddr*)&caddr,&clen);
        if(cfd == -1)
        {
            cout<<"error accept!"<<endl;
            exit(-1);
        }
        cout<<"\t\t-------------------------------\t\t"<<endl;
        cout<<"\t\t|WELCOME TO MYSCP APPLICATION!|\t\t"<<endl;
        cout<<"\t\t|NOW PREPARE TO RECIEVE MSG!  |\t\t"<<endl;
        cout<<"\t\t|PLEASE WAIT................  |\t\t"<<endl;
        cout<<"\t\t-------------------------------\t\t"<<endl;
        //对链接进行处理
        //接收相关信息
        while(rec == 0)//接收到相关信息
        {
            static int index = 0;
            cout<<"\t\t-------------------------------\t\t"<<endl;
            cout<<"\t\t|PLEASE WAIT FOR CLIENT SEND MSG...\t\t"<<endl;
            cout<<"\t\t-------------------------------\t\t"<<endl;
            rec = recv(cfd,RD_BUF,BUF_LEN,flags);

            if(rec > 0)
            {
                cout<<"\t\tRECV MSG FOR CMD && DIR!"<<endl;
                cout<<"\t\tMSG LEN:"<<rec<<endl;
                cout<<"\t\tMSG IS:"<<RD_BUF<<endl;
                for(int i=0;i<rec;i++)
                {
                    static int ct = 0;
                    if(RD_BUF[i] != '#')//opt#store-dir#
                    {
                        process[index][ct] = RD_BUF[i];
                        ct++;
                    }
                    else
                    {
                        index++;
                        ct = 0;
                        if(index==2)
                        {
                            break;
                        }
                    }
                }
            }
            else
                rec = 0;
            if(index < 2)
            {
                cout<<"\t\t-------------------------------\t\t"<<endl;
                cout<<"\t\t|RECV ERROR FORNAT-------------\t\t"<<endl;
                cout<<"\t\t-------------------------------\t\t"<<endl;
                close(cfd);
                index = 0;
                rec = 0;//重新接收
            }
            index = 0;
        }
        cout<<"\t\t-------------------------------\t\t"<<endl;
        cout<<"\t\t|CMD IS :"<<process[0]<<"-------------------\t\t"<<endl;
        cout<<"\t\t|DIR IS :"<<process[1]<<"-------------------\t\t"<<endl;
        cout<<"\t\t-------------------------------\t\t"<<endl;
        rec = 0;//清零接收计数
        memset(RD_BUF,0,strlen(RD_BUF));//清空接收缓冲
        if( !strcmp(process[0],"-s"))//recv
        {
            //根据相关信息保存文件
            fd = open(process[1],O_CREAT|O_RDWR);
            if(fd < 0 )
            {
                cout<<"file open failed!"<<endl;
                exit(-1);
            }
            //sleep(3);//等待客户端发送文件
            do{
                //cout<<"正在写数据..."<<endl;
                //rec = recv(cfd,RD_BUF,BUF_LEN,flags);//读多少写多少
                rec = recvfrom(cfd,RD_BUF,BUF_LEN,0,NULL,NULL);
                if(rec > 0)
                    write(fd,RD_BUF,rec);
            }while(rec > 0);
            close(fd);
            sprintf(cmdbuf,"%s%s","chmod 777 ",process[1]);
            system(cmdbuf);
        }
        memset(WR_BUF,0,strlen(WR_BUF));//清空发送缓冲
        if( !strcmp(process[0],"-g"))//send
        {
            fd = open(process[1],O_RDONLY);
            if(fd < 0 )
            {
                cout<<"file open failed!"<<endl;
                exit(-1);
            }
            do{
                rec = read(fd,WR_BUF,BUF_LEN);//读多少发多少
                if(rec > 0)
                    send(cfd,WR_BUF,rec,flags);
                usleep(1000);
            }while(rec > 0);
            close(fd);
        }
        //rec = 0;
        close(cfd);
        system("/home/xny/Shell/clr.sh");
        memset(process[0],0,strlen(process[0]));
        memset(process[1],0,strlen(process[1]));
    }
    return 0;
}

//文件互传 服务端
int main(int argc,char **argv)
{
    if(argc < 3)
    {
        printf("cmd error!\ntry like:scpsvr 192.168.0.128 8888\n");
        exit(-1);
    }
    myscp(argv);
    return 0;
}