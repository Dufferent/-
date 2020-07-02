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

/*  寄存器地址 */

#define CCM_CCGR1_BASE              (0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE      (0X020E0068) 
#define SW_PAD_GPIO1_IO03_BASE      (0X020E02F4) 
#define GPIO1_DR_BASE               (0X0209C000) 
#define GPIO1_GDIR_BASE             (0X0209C004)
#define REGISTER_LENGTH             4

static void dev_release(struct device *dev)
{
    printk("dev removed!\r\n");
}

/*  设备资源信息设置 */
static struct resource dev_resource[] = {
    [0] = {
        .start = CCM_CCGR1_BASE,
        .end   = (CCM_CCGR1_BASE + REGISTER_LENGTH - 1),
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start = SW_MUX_GPIO1_IO03_BASE,
        .end   = (SW_MUX_GPIO1_IO03_BASE + REGISTER_LENGTH - 1),
        .flags  = IORESOURCE_MEM,
    },
    [2] = {
        .start = SW_PAD_GPIO1_IO03_BASE,
        .end   = (SW_PAD_GPIO1_IO03_BASE + REGISTER_LENGTH - 1),
        .flags  = IORESOURCE_MEM,
    },
    [3] = {
        .start = GPIO1_DR_BASE,
        .end   = (GPIO1_DR_BASE + REGISTER_LENGTH - 1),
        .flags  = IORESOURCE_MEM,
    },
    [4] = {
        .start = GPIO1_GDIR_BASE,
        .end   = (GPIO1_GDIR_BASE + REGISTER_LENGTH - 1),
        .flags  = IORESOURCE_MEM,
    },
};

static struct platform_device devdevice = {
    .name = "dev_mod",
    .id   =  -1,
    .dev  = {
        .release = &dev_release,
    },
    .num_resources = ARRAY_SIZE(dev_resource),
    .resource     = dev_resource,
};

static int __init device_init(void)
{
    return platform_device_register(&devdevice);
}

static void __exit device_exit(void)
{
    platform_device_unregister(&devdevice);
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xny");