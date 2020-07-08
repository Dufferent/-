#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/unistd.h"
#include "sys/stat.h"
#include "sys/syscall.h"
#include "sys/time.h"
#include "string.h"
#include "linux/ioctl.h"
#include "linux/mman.h"
#include "linux/input.h"
#include "linux/input-event-codes.h"
#include "poll.h"
#include "signal.h"


int main(int argc,char** argv)
{
    pid_t child[2];
    printf("fork test!\r\n");

    child[0] = fork();
    printf("child created %d!\r\n",getpid());

    return 0;
}