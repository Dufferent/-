#include <linux/types.h>                //类型库
#include <linux/kernel.h>               //内核操作
#include <linux/delay.h>                //延时
#include <linux/ide.h>                  //
#include <linux/module.h>               //模块操作
#include <linux/cdev.h>                 //字符设备操作
#include <linux/device.h>               //设备操作
#include <linux/gpio.h>                 //gpio操作    
#include <linux/errno.h>                //
#include <asm/mach/map.h>               //寄存器映射，地址空间映射
#include <asm/uaccess.h>                //寄存器映射，地址空间映射
#include <asm/io.h>                     //io操作
#include <linux/of.h>                   //查找节点
#include <linux/of_address.h>           //查找地址
#include <linux/of_gpio.h>              //gpio子系统
#include <linux/timer.h>                //定时器中断
#include <linux/semaphore.h>            //自旋锁
#include <linux/irq.h>                  //外部中断
#include <linux/of_irq.h>               //外部中断
#include <linux/poll.h>                 //非阻塞IO
#include <linux/wait.h>                 //非阻塞IO
#include <linux/fcntl.h>                /* 异步通知相关结构体 */

//设备号为<unsigned int>类型 提取主设备号 副设备号
#define MINNUMS         20    

#define DEV_NAME   "dev_mod"
#define DEV_NUMS    1

#define NODE_NUM    2

/* values types defines */
struct exti_st{
    int irq;
    irqreturn_t (*handler)(int,void*);
};

struct dev_mod{
    dev_t devid;
    struct cdev   dev;
    struct class  *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *dnode[NODE_NUM];
    unsigned int gpio[NODE_NUM];
    spinlock_t spin;
    int dev_state;
};

struct dev_mod chdev;
struct exti_st exti;
struct fasync_struct *async_queue;/* 异步通知句柄 */

void led_on(void)
{
    gpio_set_value(chdev.gpio[1],0);
}

void led_off(void)
{
    gpio_set_value(chdev.gpio[1],1);
}

//外部中断服务函数
static irqreturn_t key0_handler(int flag,void* dev_id)
{
    if(gpio_get_value(chdev.gpio[1]))
    {
        led_on();
    }
    else
    {
        led_off();
    }

    if(async_queue)
    {   //发送信号
        kill_fasync(&async_queue,SIGIO,POLL_IN);//可读时POLL_IN 可写POLL_OUT
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}


/* operations */
/* 驱动层open -> 应用层 open */
static int dev_open(struct inode *inode,struct file *filp)
{
    /* 判断锁的状态 */
    //自旋锁
    /* 锁用来保护dev_stat
     * 创建临界区
     * 判断dev_stat的值确定是否被占用
     */

    //自旋锁
    int ret;
    unsigned long flag;
    spin_lock_irqsave(&chdev.spin,flag);
    //临界区
    if(chdev.dev_state)
    {
        spin_unlock_irqrestore(&chdev.spin,flag);
        return -EBUSY;
    }
    chdev.dev_state++;
    //临界区
    spin_unlock_irqrestore(&chdev.spin,flag);
    
    //注册外部中断
    exti.irq = irq_of_parse_and_map(chdev.dnode[0],0);//step1::获取中断号
    //法2
    //exti.irq =  gpio_to_irq(chdev.gpio[0]);
    printk("exti irqnum:%d\r\n",exti.irq);
    //step2::申请中断
    exti.handler = key0_handler;
    ret = request_irq(exti.irq,exti.handler,IRQF_TRIGGER_FALLING,"exti_key0",&chdev);
    if(ret < 0)
    {
        printk("irq request failed!\r\n");
        return EFAULT;
    }


    filp->private_data = &chdev;/* 将设备结构体传给用户空间 */

    return 0;
}

/* 驱动层read -> 应用层 read */
static ssize_t dev_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
    return 0;
}

/* 驱动层write -> 应用层 write */
static ssize_t dev_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt)
{
    return 0;
}

/* 驱动层fasync函数 -> 应用层SIGNAL */
static int dev_fasync(int fd,struct file *filp,int on)
{
    return fasync_helper(fd,filp,on,&async_queue);
}

/* 驱动层release -> 应用层 close */
static int dev_release(struct inode *inode,struct file *filp)
{
    /* 解锁 */
    //atomic_inc(&chdev.lock);
    unsigned long flag = 0;
    spin_lock_irqsave(&chdev.spin,flag);
    if(chdev.dev_state)
    {
        chdev.dev_state--;
    }
    spin_unlock_irqrestore(&chdev.spin,flag);
    //注销中断
    free_irq(exti.irq,&chdev);

    return dev_fasync(-1,filp,0);
}

/* 设备操作函数 */
static struct file_operations dev_op = {
    .owner = THIS_MODULE,
    .open  = dev_open,
    .read  = dev_read,
    .write = dev_write,
    .release = dev_release,
    .fasync  = dev_fasync
};

static int __init dev_init(void)//驱动入口
{
    /* 初始化设备 */
    /* 寄存器映射 */
    int ret;
    struct property *proper[NODE_NUM];
    const char* str[NODE_NUM];
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

    //初始化原子锁
    //atomic_set(&chdev.lock,1);//加载驱动默认锁值为1
    //初始化自旋锁
    spin_lock_init(&chdev.spin);
    chdev.dev_state = 0;

    //获取设备树的节点
    chdev.dnode[0] = of_find_node_by_path("/exti_ts");
    chdev.dnode[1] = of_find_node_by_path("/pin_ts");
    if( (chdev.dnode[0] == NULL) || (chdev.dnode[1] == NULL) )
    {
        printk("can not find node!\r\n");
        return EFAULT;
    }
    else
    {
        /* code */
        printk("nodes have been found!\r\n");
    }
    //获取适配信息
    proper[0] = of_find_property(chdev.dnode[0],"compatible",NULL);
    proper[1] = of_find_property(chdev.dnode[1],"compatible",NULL);
    if( (proper[0] == NULL) || (proper[1] == NULL) )
    {
        printk("compatible property find failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("compatible1 = %s\r\ncompatible2 = %s\r\n", 
        (char*)proper[0]->value,
        (char*)proper[1]->value);
    }
    //获取状态信息
    ret = of_property_read_string(chdev.dnode[0],"status",&str[0]);
    ret = of_property_read_string(chdev.dnode[1],"status",&str[1]);
    if(ret < 0)
    {
        printk("status get failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("status1 is :%s\r\nstatus2 is :%s\r\n",str[0],str[1]);
    }

    //获取gpio-sys
    chdev.gpio[0] = of_get_named_gpio(chdev.dnode[0],"gpios",0);//<节点 gpio属性名称 索引>
    chdev.gpio[1] = of_get_named_gpio(chdev.dnode[1],"gpios",0);//<节点 gpio属性名称 索引>
    if( (chdev.gpio[0] < 0) || (chdev.gpio[1] < 0) )
    {
        printk("gpios get failed!\r\n");
        return EFAULT;
    }
    else
    {
        printk("gpio[1] :%d\r\ngpio[2] :%d\r\n",chdev.gpio[0],chdev.gpio[1]);
    }
    //配置GPIO
    ret = gpio_request(chdev.gpio[0],"exti test");
    ret = gpio_request(chdev.gpio[1],"pin test");
    if(ret < 0)
    {
        printk("gpio request failed!\r\n");
        return EFAULT;
    }
    ret = gpio_direction_input(chdev.gpio[0]);   //input
    ret = gpio_direction_output(chdev.gpio[1],1);//off led 
    if(ret < 0)
    {
        printk("gpios set failed!\r\n");
        return EFAULT;
    }

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
    gpio_free(chdev.gpio[0]);
    gpio_free(chdev.gpio[1]);

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
