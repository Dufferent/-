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
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/miscdevice.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/of_irq.h>

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
    //int major;
    //int minor;
    struct device_node *dnode;
    int gpio;
    unsigned int key_val;
};

struct dev_mod chdev;
struct input_dev *inputdev;

int irqnum;
static irqreturn_t irq_handler(int flag,void *dev)
{
    if(gpio_get_value(chdev.gpio) == 1)
    {
        input_report_key(inputdev,KEY_0,1);
        input_sync(inputdev);
        //chdev.key_val = 1;
        //printk("key0 press!\r\n");
    }
    else
    {
        input_report_key(inputdev,KEY_0,0);
        input_sync(inputdev);
        //chdev.key_val = 0;
        //printk("key0 release!\r\n");
    }
    
    return IRQ_HANDLED;
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

static struct miscdevice Miscdev = {
    .minor = 168,
    .name  = DEV_NAME,
    .fops  = &dev_op,
};


static int dev_probe(struct platform_device *dev)
{
    /* 初始化设备 */
    /* 寄存器映射 */
    int ret;
    struct property *proper;
    const char* str;
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

    //获取设备数张树的节点
    chdev.dnode = of_find_node_by_path("/exti_ts");
    if(chdev.dnode == NULL)
    {
        printk("can not find node!\r\n");
        return EFAULT;
    }
    else
    {
        /* code */
        printk("node has been found!\r\n");
    }
    //获取适配信息
    proper = of_find_property(chdev.dnode,"compatible",NULL);
    if(proper == NULL)
    {
        printk("compatible property find failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("compatible = %s\r\n", (char*)proper->value);
    }
    //获取状态信息
    ret = of_property_read_string(chdev.dnode,"status",&str);
    if(ret < 0)
    {
        printk("status get failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("status is :%s",str);
    }

    //获取gpio-sys
    chdev.gpio = of_get_named_gpio(chdev.dnode,"gpios",0);//<节点 gpio属性名称 索引>
    if(chdev.gpio < 0)
    {
        printk("gpio get failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("gpio :%d",chdev.gpio);
    }
    ret = gpio_request(chdev.gpio,"exti test");
    ret = gpio_direction_input(chdev.gpio);
    //irqnum = irq_of_parse_and_map(chdev.dnode,0);//注册中断
    //ret = request_irq(irqnum,irq_handler,IRQ_TYPE_EDGE_BOTH,"exti",&chdev);
    if(ret < 0)
    {
        printk("gpio set failed!\r\n");
        return EFAULT;
    }
    //misc设备注册
    ret = misc_register(&Miscdev);

    irqnum = irq_of_parse_and_map(chdev.dnode,0);//注册中断
    ret = request_irq(irqnum,irq_handler,IRQ_TYPE_EDGE_BOTH,"exti",&chdev);
    if(ret < 0)
    {
        printk("exti request failed!\r\n");
        return EFAULT;
    }
    /* 申请输入事件 */
    inputdev = input_allocate_device();
    inputdev->name = "INPUT_SYS";
    inputdev->evbit[0] = BIT_MASK(EV_KEY)|BIT_MASK(EV_REP);
    //inputdev->keybit[BIT_WORD(KEY_0)] |= BIT_MASK(KEY_0);
    input_set_capability(inputdev,EV_KEY,KEY_0); 
    ret = input_register_device(inputdev);
    if(ret < 0)
    {
        printk("input event request failed!\r\n");
        return EFAULT;
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
    free_irq(irqnum,&chdev);
    input_unregister_device(inputdev);
    input_free_device(inputdev);
    misc_deregister(&Miscdev);
    
    return 0;
}

static const struct of_device_id dev_of_mach[] ={
    {.compatible = "exti_ts"},
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
