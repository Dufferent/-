#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/errno.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>

/* functions def */
//设备号为<unsigned int>类型 提取主设备号 副设备号
#define MINNUMS         20    
//#define MAJOR(x)        (unsigned int)(x >> MINNUMS)
//#define MINOR(x)        (unsigned int)(x & ( (0x1 << MINNUMS) - 1) )
//#define MKDEV(mj,mi)    (unsigned int)( (mj << MINNUMS) | mi )

#define DEV_NAME   "dev_mod"
#define DEV_NUMS    1

/* 寄存器物理地址 
 *  eg:#define  CCM_CCGR1_BASE               (0X020C406C)
 */
/* 映射后的寄存器虚拟地址指针 
 *  eg: static void __iomem *IMX6U_CCM_CCGR1; 
 */

static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;



/* values types defines */
struct dev_mod{
    dev_t devid;
    struct cdev   dev;
    struct class  *class;
    struct device *device;
    int major;
    int minor;
};

struct dev_mod chdev;

/* led drv */
void led_on(void)
{
    writel( readl(GPIO1_DR) & ( ~(0x1<<3) ) ,GPIO1_DR);
}

void led_off(void)
{
    writel( readl(GPIO1_DR) | (0x1<<3) ,GPIO1_DR);
}

/* operations */
/* 驱动层open -> 应用层 open */
static int dev_open(struct inode *inode,struct file *filp)
{
    filp->private_data = &chdev;/* 将设备结构体传给用户空间 */
    return 0;
}

/* 驱动层read -> 应用层 read */
static ssize_t dev_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
    /* code */
    return 0;
}

/* 驱动层write -> 应用层 write */
static ssize_t dev_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt)
{
    /* code */
    int ret;
    unsigned char data_buf[1];
    /* code */
    ret = copy_from_user(data_buf,buf,cnt);
    if(ret < 0)
    {
        printk("write error!\r\n");
        return EFAULT;
    }
    if(data_buf[0] == '1')
    {
        printk("led off!\r\n");
        led_off();
    }
    else if(data_buf[0] == '2')
    {
        printk("led on!\r\n");
        led_on();
    }
    return 0;
}

/* 驱动层release -> 应用层 close */
static int dev_release(struct inode *inode,struct file *filp)
{
    return 0;
}

/* 设备操作函数 */
static struct file_operations dev_op = {
    .owner = THIS_MODULE,
    .open  = dev_open,
    .read  = dev_read,
    .write = dev_write,
    .release = dev_release
};

static int dev_probe(struct platform_device *dev)
{
    struct resource *dev_resource[5];
    int size_of[5];
    int i = 0;
    unsigned int val;
    
    for(i = 0;i < 5;i++ )
    {
        dev_resource[i] = platform_get_resource(dev,IORESOURCE_MEM,i);
        size_of[i]      = resource_size(dev_resource[i]);
    }

    IMX6U_CCM_CCGR1   = ioremap(dev_resource[0]->start,size_of[0]);
    SW_MUX_GPIO1_IO03 = ioremap(dev_resource[1]->start,size_of[1]);
    SW_PAD_GPIO1_IO03 = ioremap(dev_resource[2]->start,size_of[2]);
    GPIO1_DR          = ioremap(dev_resource[3]->start,size_of[3]);
    GPIO1_GDIR        = ioremap(dev_resource[4]->start,size_of[4]);

    /* 硬件初始化 */
    //时钟初始化
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(0x3<<26);//to be zero
    writel(val,IMX6U_CCM_CCGR1);
    val |=  (0x3<<26);//enable clk gpio1 
    writel(val,IMX6U_CCM_CCGR1);
    /* 配置管脚 */
    writel(0x05,SW_MUX_GPIO1_IO03);//AF GPIO1_IO3
    writel(0x10B0,SW_PAD_GPIO1_IO03);//IO format out
    writel(readl(GPIO1_GDIR)|(0x1<<3),GPIO1_GDIR);//IO format out
    writel(readl(GPIO1_DR)&(~(0x1<<3)),GPIO1_DR);//init on

    /* region chdev */
    /* 申请设备号 */
    alloc_chrdev_region(&chdev.devid,0,DEV_NUMS,DEV_NAME); 
    chdev.major = MAJOR(chdev.devid);//获取主设备号
    chdev.minor = MINOR(chdev.devid);//获取副设备号
    printk("newcheled major=%d,minor=%d\r\n", chdev.major, chdev.minor);

    /* 初始化 struct dev_mod chdev */
    chdev.dev.owner = THIS_MODULE;
    cdev_init(&chdev.dev,&dev_op);
    cdev_add(&chdev.dev,chdev.devid,DEV_NUMS);

    /* 注册设备 */
    chdev.class = class_create(THIS_MODULE,DEV_NAME);
    if(IS_ERR(chdev.class))
    {
        return PTR_ERR(chdev.class);
    }
    chdev.device = device_create(chdev.class,NULL,chdev.devid,NULL,DEV_NAME);
    if(IS_ERR(chdev.device))
    {
        return PTR_ERR(chdev.device);
    }


    return 0;
}

static int dev_remove(struct platform_device *dev)
{
    /* 取消映射 */
    /* EG:
     *  iounmap(IMX6U_CCM_CCGR1); 
     */

    /* 注销设备 */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03); 
    iounmap(SW_PAD_GPIO1_IO03); 
    iounmap(GPIO1_GDIR); 
    iounmap(GPIO1_DR); 

    cdev_del(&chdev.dev);
    unregister_chrdev_region(chdev.devid,DEV_NUMS);
    device_destroy(chdev.class,chdev.devid);
    class_destroy(chdev.class);
    return 0;
}

static const struct of_device_id dev_of_mach[] ={
    {.compatible = "dev_mod"},
    { /* adds */ }
};

static struct platform_driver plat_dev = {
    .driver = {
        .name = "dev_mod",
        .of_match_table = dev_of_mach
    },
    .probe = dev_probe,
    .remove = dev_remove,
};

static int __init platdev_init(void)
{
    return platform_driver_register(&plat_dev);
}

static void __exit platdev_exit(void)
{   
    platform_driver_unregister(&plat_dev);
}

/* module end */
module_init(platdev_init);
module_exit(platdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xny");
