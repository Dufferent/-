#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "sys/ioctl.h"

/* fb框架 */
#include "linux/fb.h"

/* 屏幕信息结构体 */
struct fb_fix_screeninfo fb_fix;
struct fb_var_screeninfo fb_var;

/* 屏幕信息 */
#define SCREEN_SIZE     500*300
#define SCREEN_WIDTH    500
#define SCREEN_HEIGHT   300
#define PIXEL           16          
#define PIXEL_BYTE      2

/* RGB888 -> RGB565 */
unsigned short int rgb_rechange(unsigned int color_888)
{
    unsigned short int color_565;
    color_565 = (((((color_888) >> 19) & 0x1f) << 11)|((((color_888) >> 10) & 0x3f) << 5)|(((color_888) >> 3) & 0x1f));
    return color_565;
}

/* 屏幕操作函数 */
void fill_screen(unsigned int color,unsigned char *addr)
{
    unsigned char col_h = rgb_rechange(color)>>8;
    unsigned char col_l = rgb_rechange(color);
    for(int i=0;i<SCREEN_SIZE;i++)
    {
        memcpy(addr,&col_h,1);
        addr++;
        memcpy(addr,&col_l,1);
        addr++;
    }
}

void fill_screen_short(unsigned int color,unsigned short int *addr)
{
    for(int i=0;i<SCREEN_SIZE;i++)
    {
        *addr = rgb_rechange(color);
        addr++;
    }
}

int main(int argc,char* argv[])
{
    /* 文件描述符 */
    int fd;
    char *dev;
    int ret;
    //unsigned char *frame;
    unsigned short int *frame;


    /* 调色盘 */
    unsigned int yellow = 0xffff00;//<R G B 8 8 8>
    unsigned int red    = 0xff0000;//<R G B 8 8 8>
    unsigned int green  = 0x00ff00;//<R G B 8 8 8>
    unsigned int blue   = 0x0000ff;//<R G B 8 8 8>
    unsigned int gray   = 0x202020;

    /* 获取设备名称 */
    dev = (char*)malloc(sizeof(char)*10);
    sprintf(dev,"%s",argv[1]);
    printf("dev name is \"%s\"\r\n",dev);
    /* 第一步：打开设备 */
    fd = open(dev , O_RDWR);//阻塞，读写
    if( fd < 0 )
    {
        printf("dev open failed!\r\n");
        close(fd);//关闭设备
        exit(0);
    }
    
    /* 第二步：获取屏幕信息 */
    ret = ioctl(fd,FBIOGET_VSCREENINFO,&fb_var);
    if( ret < 0)
    {
        printf("acquire screen info failed!\r\n");
        close(fd);//关闭设备
        exit(0);
    }
    //打印设备固化信息
    printf("LCD WIDTH  :%d\r\n",fb_var.xres);
    printf("LCD HEIGHT :%d\r\n",fb_var.yres);
    printf("pix size   :%d\r\n",fb_var.bits_per_pixel);

    /* 内存映射 */
    //frame = (unsigned char *)mmap(NULL,SCREEN_SIZE*2,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    frame = (unsigned short int*)mmap(NULL,SCREEN_SIZE*2,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if( frame == NULL )
    {
        printf("mmap failed!\r\n");
        free(dev);
        close(fd);
        exit(0);
    }

    /* 操作显存 */
    //fill_screen(atoi(argv[2]),frame);
    fill_screen_short(atoi(argv[2]),frame);
    /* 具体功能 */

    /* 关闭设备 */
    close(fd);

    return 0;
}