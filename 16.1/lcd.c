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

/* 字库 */
//标准LCD字库
#include <font.h>

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

/* screen functions */
//#####################################################################
/*
 * 适用于 screen-size = 800*480
 *       pixel-deep  = 24
 *       TFT-LCD
 */
#define graph_nums 1
unsigned int ret_get[graph_nums][800*480] = {0};	//转化为32位的像素信息
unsigned char get[graph_nums][800*480*3 + 54] = {0};//读取的图片像素信息
unsigned int *resize_buf;
//画点函数
void draw_point(unsigned int *p, unsigned int color, int pos_x, int pos_y)
{
	//memcpy(p+pos_x+pos_y*SCREEN_WIDTH, &color, PIXEL_BYTE);
	*(p+pos_x+SCREEN_WIDTH*pos_y) = color;
}
//处理bmp图片获取图片数据
void process_get(unsigned char addr[],int t)
{
	int j = 0;
	for(int i = SCREEN_HEIGHT-1;i>=0;i--)
	for(int n = 0;n<SCREEN_WIDTH;n++)
	{
		ret_get[t][j] = (unsigned int)(addr[(i*SCREEN_WIDTH+n)*PIXEL_BYTE+54] + addr[(i*SCREEN_WIDTH+n)*PIXEL_BYTE+54+1]*256 + addr[(i*SCREEN_WIDTH+n)*PIXEL_BYTE+54+2]*256*256);/* 头部不要了 */
		j++;
	}
}
//画图
void draw_bmp(unsigned int *p,unsigned int bmp[])
{
	for(int j = 0; j<SCREEN_HEIGHT; j++)
	for(int i = 0; i<SCREEN_WIDTH; i++)
	{
		draw_point(p,bmp[i+SCREEN_WIDTH*j],i,j);
	}
}
/*
width support:SCREEN_WIDTH的公因数（整数）
height support:SCREEN_HEIGHT的公因数（整数）
*/
void img_resize(unsigned int *p,int width,int height)
{
	static int flag = 0;
	static int k=0;
	if(!flag)
		{resize_buf = (unsigned int*)malloc(sizeof(unsigned int)*width*height);flag = 1;}
	for(int j=0;j<SCREEN_HEIGHT;)
	{
		for(int i=0;i<SCREEN_WIDTH;)
		{
			resize_buf[k] = p[j*SCREEN_WIDTH+i];
			k++;

			i+= (SCREEN_WIDTH/width);
		}
		j+= (SCREEN_HEIGHT/height);
	}
	k=0;
}
//任意区域画图
void draw_bmp_free(unsigned int *p,unsigned int *res_img,int width,int height,int s_x,int s_y)
{
	static int k=0;
	img_resize(res_img,width,height);
	for(int y=s_y; y<s_y+height ; y++ )
		for(int x=s_x; x<s_x+width ; x++)
		{
			memcpy(p+x+y*SCREEN_WIDTH, &resize_buf[k++], PIXEL_BYTE);
		}
	k=0;
}
//清屏函数
void clear_free(unsigned int *p,int width,int height,int s_x,int s_y)
{
	unsigned int color = 0;
	for(int y=s_y; y<s_y+height ; y++ )
		for(int x=s_x; x<s_x+width ; x++)
		{
			memcpy(p+x+y*SCREEN_WIDTH, &color, PIXEL_BYTE);
		}
}

void clear(unsigned int *p)
{
	unsigned int color = 0x0;
	for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++)
	{
		memcpy(p+i, &color, PIXEL_BYTE);		
	}
}

/*
 * 字库相关操作函数
 */

void lcd_showchar(unsigned int *p,int x, int y,
				      unsigned char num, unsigned char size, 
				      unsigned char mode)
{  							  
    unsigned char  temp, t1, t;
	unsigned short y0 = y;
	unsigned char csize = (size / 8+ ((size % 8) ? 1 : 0)) * (size / 2);	/* 得到字体一个字符对应点阵集所占的字节数	 */	
 	num = num - ' ';  	/*得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）  */
	for(t = 0; t < csize; t++)
	{   
		if(size == 12) temp = asc2_1206[num][t]; 		/* 调用1206字体 */
		else if(size == 16)temp = asc2_1608[num][t];	/* 调用1608字体 */
		else if(size == 24)temp = asc2_2412[num][t];	/* 调用2412字体 */
		else if(size == 32)temp = asc2_3216[num][t];	/* 调用3216字体 */
		else return;									/* 没有的字库 		*/
		for(t1 = 0; t1 < 8; t1++)
		{			    
			if(temp & 0x80)draw_point(p, 0x00ffffff, x, y);
			else if(mode==0)draw_point(p, 0x0, x, y);
			temp <<= 1;
			y++;
			if(y >= 480) return;			/* 超区域了 */	
			if((y - y0) == size)
			{
				y = y0;
				x++;
				if(x >= 800) return;		/* 超区域了 */
				break;
			}
		}  	 
	}  		    	   	 	  
} 
unsigned int lcd_pow(unsigned char m,unsigned char n)
{
	unsigned int result = 1;	 
	while(n--) result *= m;    
	return result;
}

void lcd_shownum(unsigned int *p,
				 int x, 
				 int y, 
				 unsigned int num, 
				 unsigned char len,
				 unsigned char size)
{         	
	unsigned char  t, temp;
	unsigned char  enshow = 0;						   
	for(t = 0; t < len; t++)
	{
		temp = (num / lcd_pow(10, len - t - 1)) % 10;
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
				lcd_showchar(p, x + (size / 2) * t, y, ' ', size, 0);
				continue;
			}else enshow = 1; 	 
		}
	 	lcd_showchar(p, x + (size / 2) * t, y, temp + '0', size, 0); 
	}
} 

void lcd_show_string(unsigned int *fb,int x,int y,
						  unsigned short width,unsigned short height,
						  unsigned char size,char *p)
{         
	unsigned char x0 = x;
	width += x;
	height += y;
    while((*p <= '~') &&(*p >= ' '))		/* 判断是不是非法字符! */ 
    {       
        if(x >= width) {x = x0; y += size;}
        if(y >= height) break;				/* 退出 */
        lcd_showchar(fb, x, y, *p , size, 0);
        x += size / 2;
        p++;
    }  
}


//###############################################################

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