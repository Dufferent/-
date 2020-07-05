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
#define IMG_WIDTH       640
#define IMG_HEIGHT      480
#define IMG_PIXEL       2


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

/* rgb888 to rgb565 */
unsigned short int rgb_rechange(unsigned int color_888)
{
    unsigned short int color_565;
    color_565 = (((((color_888) >> 19) & 0x1f) << 11)|((((color_888) >> 10) & 0x3f) << 5)|(((color_888) >> 3) & 0x1f));
    return color_565;
}
/* yuyv to rgb888 */
#define RGB24_MAX(x) (x > 255)? 255  : x 
#define RGB24_MIN(x) (x < 0)  ? 0    : x
char recbuf[IMG_WIDTH*IMG_HEIGHT*3] = {0};
void yuyv_to_rgb888(char *yuyv)
{
    int pix;
    int rgb_pix = 0;
    unsigned char y1,u1,y2,v1;
    int r1,g1,b1,r2,g2,b2;
    //像素空间
    for(pix=0;pix<IMG_WIDTH*IMG_HEIGHT*2;pix+=4)
        {
            y1 = yuyv[pix];
            u1 = yuyv[pix + 1];
            y2 = yuyv[pix + 2];
            v1 = yuyv[pix + 3];

            /* to rgb 24 */
            r1 = y1 + 1.370705*(v1-128);
            g1 = y1 - 0.698001*(v1-128) - 0.337633*(u1-128);
            b1 = y1 + 1.732446*(u1-128);
            r2 = y2 + 1.370705*(v1-128);
            g2 = y2 - 0.698001*(v1-128) - 0.337633*(u1-128);
            b2 = y2 + 1.732446*(u1-128);

            r1 = (RGB24_MAX(r1));
            g1 = (RGB24_MAX(g1));
            b1 = (RGB24_MAX(b1));
            r2 = (RGB24_MAX(r2));
            g2 = (RGB24_MAX(g2));
            b2 = (RGB24_MAX(b2));
            r1 = (RGB24_MIN(r1));
            g1 = (RGB24_MIN(g1));
            b1 = (RGB24_MIN(b1));
            r2 = (RGB24_MIN(r2));
            g2 = (RGB24_MIN(g2));
            b2 = (RGB24_MIN(b2));

            recbuf[rgb_pix]     = r1;
            recbuf[rgb_pix + 1] = g1;
            recbuf[rgb_pix + 2] = b1;
            recbuf[rgb_pix + 3] = r2;
            recbuf[rgb_pix + 4] = g2;
            recbuf[rgb_pix + 5] = b2;
            rgb_pix+=6;
        }
}

/* BMP GRAPH */
/* bmp img structor */
struct{
     u_int16_t    bfType;                // the flag of bmp, value is "BM"
     u_int32_t    bfSize;                // size BMP file ,unit is bytes
     u_int32_t    bfReserved;            // 0
     u_int32_t    bfOffBits;             // must be 54
}bmphead;

struct{
     u_int32_t    biSize;            // must be 0x28
     u_int32_t    biWidth;           //
     u_int32_t    biHeight;          //
     u_int16_t    biPlanes;          // must be 1
     u_int16_t    biBitCount;        //
     u_int32_t    biCompression;     //
     u_int32_t    biSizeImage;       //
     u_int32_t    biXPelsPerMeter;   //
     u_int32_t    biYPelsPerMeter;   //
     u_int32_t    biClrUsed;         //
     u_int32_t    biClrImportant;    //
}bmpbody;



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
    //req_buf只是用来设置请求的缓冲区的，后面用到的实际缓冲区是v4l2_buffer
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


u_int8_t capture_start(int *cap)
{
    int on = V4L2_BUF_TYPE_VIDEO_CAPTURE;//采集开启标志位
    int ret;
    ret = ioctl(*cap,VIDIOC_STREAMON,&on);
    if(-1 == ret)
    {
        printf("开启采集失败!\r\n");
        close(*cap);
        return 0;
    }
    return 1;
}

u_int8_t capture_stop(int *cap)
{
    int off = V4L2_BUF_TYPE_VIDEO_CAPTURE;//采集关闭标志位
    int ret;
    ret = ioctl(*cap,VIDIOC_STREAMOFF,&off);
    if(-1 == ret)
    {
        printf("停止采集失败!\r\n");
        close(*cap);
        return 0;
    }
    return 1;
}
/************************************************************
 struct timeval tv;
    fd_set fds;

    
    FD_ZERO(&fds);
    FD_SET(*cap, &fds);
    tv.tv_sec = 2;//time out
	tv.tv_usec = 0;
	ret = select(*cap+1, &fds, NULL, NULL, &tv);//判断摄像头是否准备好，tv是定时
    if(-1 == ret){
        printf("select erro! \n");
	}
	else if(0 == ret){
		printf("select timeout! \n");//超时
		return 1;
	}
**************************select轮询******************************/
/*
void BMP_INIT(bmphead *head,bmpbody *body)
{
    body->biBitCount=24;
    body->biClrImportant=0;
    body->biClrUsed=0;
    body->biCompression=0;
    body->biHeight=IMG_HEIGHT;
    body->biPlanes=1;
    body->biSize=40;
    body->biSizeImage=IMG_HEIGHT*IMG_WIDTH*3;
    body->biWidth=IMG_WIDTH;
    body->biXPelsPerMeter=0;
    body->biYPelsPerMeter=0;

    head->bfOffBits=54;
    head->bfReserved=0;
    head->bfSize= 54 + (body->biSizeImage);
    head->bfType=0x4d42;
}
*/

/*
u_int8_t BMP_OUT(int *cap)
{
    FILE *ptr;
    char filename[50] = {0};

    ptr = fopen(filename,"a+");
    if(NULL == ptr)
    {
        printf("file operation failed!\r\n");
        close(*cap);
        fclose(ptr);
        return false;
    }
    yuyv_to_rgb888(date[current_buf.index]);
    BMP_INIT(&bmph,&bmpb);
    fwrite(&bmph,14,1,ptr);
    fwrite(&bmpb,40,1,ptr);
    fwrite(recbuf,current_buf.bytesused*3,1,ptr);
    fclose(ptr);
}
*/