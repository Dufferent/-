#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "signal.h"

int fd;//文件描述符

static void sigio_signal_func(int signum)
{
	int ret = 0;
	int val = 0;
	ret = read(fd ,&val, sizeof(int));
	if(ret < 0)
	{
		printf("read failed!\r\n");
	}
	else
	{
		printf("key0 pressed\r\n");
	}
}

int main(int argc,char* argv[])
{
	unsigned char KEY_VALUE;
	unsigned int flags = 0;

	/*
	if(argc < 3)
	{
		printf("error:cmd less!\r\n");
		return -1;
	}
	*/

	fd = open(argv[1],O_RDWR);
	if(fd < 0)
	{
		printf("open failed!\r\n");
		exit(-1);
	}
	
	/* 设置信号通知 */
	signal(SIGIO,sigio_signal_func);//接收到驱动发来的相应信号执行后续处理函数
	fcntl(fd,F_SETOWN,getpid());//告诉内核当前的进程号
	flags = fcntl(fd,F_GETFD);  //获取当前的进程状态
	fcntl(fd,F_SETFL,flags | FASYNC);//设置改进程启用异步通知,驱动中的fasync函数执行

	while(1)
	{
		sleep(2);
	}

	close(fd);

	return 0;
}
