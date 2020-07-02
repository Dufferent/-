#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"

int main(int argc,char* argv[])
{
	int fd;
	unsigned char KEY_VALUE;
	if(argc < 3)
	{
		printf("error:cmd less!\r\n");
		return -1;
	}

	fd = open(argv[1],O_RDWR);
	if(fd < 0)
	{
		printf("open failed!\r\n");
		exit(-1);
	}
	while(1)
	{
		static int ct = 0; 
		read(fd,&KEY_VALUE,1);
		if(KEY_VALUE == 0)
		{
			printf("key0 press!\r\n");
			ct++;
		}
		if(ct > 5)
		{
			break;
		}
	}

	close(fd);

	return 0;
}
