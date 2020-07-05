#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "sys/ioctl.h"

/* v4l2 框架 */
#include "linux/videodev2.h"

/* 图像信息 */
#define COUNT 4
unsigned short *frame_buf;//YUVY-422 8bit 1byte

/* v4l2 设备结构体 */
struct v4l2_capability cap;

struct v4l2_format cap_fmt;
struct v4l2_fmtdesc cap_fmtdesc;

struct v4l2_requestbuffers cap_reqbuf;//请求的帧缓冲
struct v4l2_buffer cap_buf;//储存的帧缓冲


struct Buffer{
    int type;
    unsigned short *start;
};
struct Buffer buffer[COUNT];
/* 设备信息 */
//...


int main(int argc,char *argv[])
{
    /* 设备描述符 */
    int fd;
    int ret;



    /* 第一步：打开设备 */
    fd = open(argv[1],O_RDWR);//阻塞，读写
    if(fd<0)
    {
        printf("cap open failed!\r\n");
        close(fd);
        exit(0);
    }

    /* 第二步：获取设备信息 */
    ret = ioctl(fd,VIDIOC_QUERYCAP,&cap);//设备信息
    if(ret == -1)
    {
        printf("acquire cap info failed!\r\n");
        close(fd);
        exit(0);
    }
    printf("versions:%d\r\n",cap.version);
    printf("driver:%s\r\n",cap.driver);
    printf("support:%#x\r\n",cap.capabilities);
    printf("card:%s\r\n",cap.card);
    printf("buf-info:%s\r\n",cap.bus_info);

    if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE == V4L2_CAP_VIDEO_CAPTURE)
        printf("cap support capture mode!\r\n");
    if(cap.capabilities & V4L2_CAP_STREAMING == V4L2_CAP_STREAMING)
        printf("cap support stream mode!\r\n");
    //编码支持 < 必须设置type和索引 >
    cap_fmtdesc.index = 0;
    cap_fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while(ioctl(fd,VIDIOC_ENUM_FMT,&cap_fmtdesc) != -1)
    {
        printf("\tsuport fmt:%s\r\n",cap_fmtdesc.description);
        cap_fmtdesc.index++;
    }
    //图像信息
    cap_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd,VIDIOC_G_FMT,&cap_fmt);
    if(ret == -1)
    {
        printf("acquire cap fmt failed!\r\n");
        close(fd);
        exit(0);
    }
    printf("\t\twidth:%d\r\n",cap_fmt.fmt.pix.width);
    printf("\t\theight:%d\r\n",cap_fmt.fmt.pix.height);
    printf("\t\tsize-img:%d\r\n",cap_fmt.fmt.pix.sizeimage);
    printf("\t\tline-bytes:%d\r\n",cap_fmt.fmt.pix.bytesperline);
    printf("\t\tpixel_deep:%d\r\n",cap_fmt.fmt.pix.bytesperline/cap_fmt.fmt.pix.width*8);


    /* 设置cap */
    /* code 根据自己需要 可以不设置 */

    /* 第三步：请求帧缓冲区域 */
    cap_reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cap_reqbuf.count = COUNT;//请求4帧
    cap_reqbuf.memory = V4L2_MEMORY_MMAP;
    ioctl(fd,VIDIOC_REQBUFS,&cap_reqbuf);
    if(ret<0)
    {
        printf("request buf failed!\r\n");
        close(fd);
        exit(0);
    }
    /* 第四步：帧缓冲区加入图像采集队列，将视屏数据空间映射到用户空间 */
    cap_buf.index = 0;
    cap_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(int i=0;i<COUNT;i++)
    {
        //入队
        ret = ioctl(fd,VIDIOC_QUERYBUF,&cap_buf);
        if(ret == -1)
        {
            printf("insert buf failed!\r\n");
            close(fd);
            exit(0);
        }
        /* 内存映射 */
        buffer[cap_buf.index].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer[cap_buf.index].start = (unsigned short*)mmap(NULL,
                                                        cap_fmt.fmt.pix.height*cap_fmt.fmt.pix.bytesperline,
                                                        PROT_READ | PROT_WRITE,
                                                        MAP_SHARED,
                                                        fd,cap_buf.m.offset);
        if(buffer[cap_buf.index].start == MAP_FAILED)
        {
            printf("buf[%d] map failed!\r\n",cap_buf.index);
            close(fd);
            exit(0);
        }
        //列队
        ret = ioctl(fd,VIDIOC_QBUF,&cap_buf);
        if(ret == -1)
        {
            printf("list buf failed!\r\n");
            close(fd);
            exit(0);
        }
        cap_buf.index++;
    }

    /* 第五步：开启视屏流 */
    enum v4l2_buf_type type;
    type = V4L2_CAP_VIDEO_CAPTURE;
    ret = ioctl(fd,VIDIOC_STREAMON,&type);
    if(ret == -1)
    {
        printf("stream on failed!\r\n");
        close(fd);
        exit(0);
    }

    /* 第六步：弹出已经存储好信息的队列 */
    for(int i=0;i<COUNT;i++)
    {
        cap_buf.index = COUNT;
        ret = ioctl(fd,VIDIOC_DQBUF,&cap_buf);
        if(ret != -1)
        {
            FILE *ptr;
            char filename[15];
            sprintf(filename,"%s%d%s","./cap",i,".yuv");
            ptr = fopen(filename,"a+");
            if(ptr == NULL)
            {
                printf("file open failed\r\n");
                close(fd);
                fclose(ptr);
                system("rm ./cap.yuvy");
                exit(0);
            }
            fwrite(buffer[i].start,
            cap_fmt.fmt.pix.height*cap_fmt.fmt.pix.bytesperline,
            1,
            ptr);
            fclose(ptr);
        }
    }

    /* 最后：关闭设备 */
    close(fd);
    return 0;
}


