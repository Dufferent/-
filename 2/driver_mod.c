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
        printk("user write success!\r\n");
    }
    else if(data_buf[0] == '2')
    {
        printk("XNY USER ROOM!\r\n");
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

static int __init dev_init(void)//驱动入口
{
    /* 初始化设备 */
    /* 寄存器映射 */
    /*
     * EG:IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4); 
     */
    /* 是能时钟 */
    /* EG: unsigned int val;
     * val = readl(IMX6U_CCM_CCGR1);
     * val &= x<<xx;
     * cal |= x<<xx;
     *       writel(val,IMX6U_CCM_CCGR1)
     */
     
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

static void __exit dev_exit(void)//驱动出口
{
    /* 取消映射 */
    /* EG:
     *  iounmap(IMX6U_CCM_CCGR1); 
     */

    /* 注销设备 */
    cdev_del(&chdev.dev);
    unregister_chrdev_region(chdev.devid,DEV_NUMS);
    device_destroy(chdev.class,chdev.devid);
    class_destroy(chdev.class);
}

/* module end */
module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xny");
