#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"

int main(int argc,char* argv[])
{
	int fd;
	if(argc < 3)
	{
		printf("error:cmd less!\r\n");
		return -1;
	}

	fd = open("/dev/dev_mod",O_RDWR);
	if(fd < 0)
	{
		printf("open failed!\r\n");
		exit(-1);
	}
	write(fd,argv[2],1);

	/* 模拟占用25s */
	do{
		static short ct = 4;
		sleep(10);
		printf("run times %d\r\n",5-ct);
		ct--;

		if((ct) == 0)
		{
			break;
		}
	}while(1);

	close(fd);

	return 0;
}
