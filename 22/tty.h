#ifndef __TTY_H
#define __TTY_H

#include "poll.h"
#include "signal.h"

struct termios *set;
struct pollfd p_fd;
nfds_t fds_t;
/* types */
typedef unsigned char u8;
typedef unsigned int  u32;

/* success return 0 else return -1 */
int open_tty(int fd,char* filename);
int close_tty(int fd);
int init_tty(int fd,u32 baudrate,u8 data,u8 stop,u8 parbit);
u8  rec_byte_tty(int fd);
int rtx_byte_tty(int fd,char rtx_buf);

#endif