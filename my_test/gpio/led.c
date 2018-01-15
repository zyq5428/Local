#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/device.h>

#include "led.h"

struct cdev led_dev;
dev_t devno;

int led_open (struct inode * node, struct file *filp)
{
	unsigned int data;
	
	/*set RXD3 and TXD3 output*/
    led_config = ioremap(GPMCON,4);
	
	data = readl(led_config);
	data &= ~(0xff << 8);
	data |= (0x1 << 8) | (0x1 << 12);
    writel(data, led_config);
	
	/*read GPBCON register*/
	data = readl(led_config);
	printk(KERN_WARNING"GPBCON value is %u", data);

    led_data = ioremap(GPMDAT,4);
    return 0;
}

long led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned int data;
	
    switch (cmd) {
        case LED_ON:
			data = readl(led_data);
			data |= (0xc << 0);
			writel(data, led_data);

            return 0;

        case LED_OFF:
			data = readl(led_data);
			data &= ~(0xc << 0);
			writel(data, led_data);

            return 0;

        default:
            return -EINVAL;
    }
}

static struct file_operations led_fops = 
{
    .open = led_open,
    .unlocked_ioctl = led_ioctl,
};

static int led_init(void)
{
	struct class *led_class;
	
    cdev_init(&led_dev, &led_fops);

    alloc_chrdev_region(&devno, 0, 1, "myled");

    cdev_add(&led_dev, devno, 1);
	
	led_class = class_create(THIS_MODULE, "led_class");
	device_create(led_class, NULL, devno, NULL,"myled");
	
    return 0;
}


static void led_exit(void)
{
    cdev_del(&led_dev);
    unregister_chrdev_region(devno, 1);
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("JOHNSON");
MODULE_LICENSE("Dual BSD/GPL");
