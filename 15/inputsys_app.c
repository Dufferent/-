#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "string.h"
#include "poll.h"
#include "sys/select.h"
#include "sys/time.h"
#include "signal.h"
#include "linux/input.h"

struct input_event inputevt;

int main(int argc,char* argv[])
{
	int fd;
	int ret;

	fd = open(argv[1],O_RDWR);
	if(fd < 0)
	{
		printf("open failed!\r\n");
		exit(-1);
	}
	
	while(1)
	{
		ret = read(fd,&inputevt,sizeof(inputevt));
		printf("type: %d ,code: %#x ,value %d\r\n",inputevt.type,inputevt.code,inputevt.value);
		sleep(2);
		printf("input read finished!\r\n");
	}


	close(fd);

	return 0;
}
