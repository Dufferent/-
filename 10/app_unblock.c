#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "poll.h"
#include "sys/select.h"
#include "sys/time.h"
#include "linux/ioctl.h"
#include "sys/mman.h"

int main(int argc,char* argv[])
{
	int fd;
	int ct = 0;
	struct pollfd fdp;
	fd_set r_fdp;
	struct timeval timout;
	unsigned int KEY_VALUE = 0;

	/*
	if(argc < 3)
	{
		printf("error:cmd less!\r\n");
		return -1;
	}
	*/

	fd = open(argv[1],O_RDWR | O_NONBLOCK);
	if(fd < 0)
	{
		printf("open failed!\r\n");
		exit(-1);
	}
	
	fdp.fd = fd;
	fdp.events = POLLIN;

	while(1)
	{
		/*
		static int ct = 0;
		read(fd,&KEY_VALUE,sizeof(KEY_VALUE));
		if(KEY_VALUE == 1)
		{
			ct++;
			KEY_VALUE = 0;
		}
		if(ct >= 5)
			break;
		*/
		static int ret = 0;
		ret = poll(&fdp,1,500); //<poll句柄 数量 超时>
		if(ret)
		{
			ret = read(fd,&KEY_VALUE,sizeof(KEY_VALUE));
			if(KEY_VALUE)
			{
				printf("key0 press!\r\n");
				ct++;
				if(ct >= 5)
					break;
			}
		}
	}

	close(fd);

	return 0;
}
