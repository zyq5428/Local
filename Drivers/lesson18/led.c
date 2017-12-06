#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>

#include "led.h"

struct cdev led_dev;
dev_t devno;

int led_open (struct inode * node, struct file *filp)
{
	led_config = ioremap(GPMCON,4);
	writel(0x00001111,led_config);
	
	led_data = ioremap(GPMDAT,4);
	return 0;
}

long led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case LED_ON:
			writel(0xf0,led_data);
			return 0;
			
		case LED_OFF:
			writel(0xff,led_data);
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
	cdev_init(&led_dev, &led_fops);
	
	alloc_chrdev_region(&devno, 0, 1, "myled");

	cdev_add(&led_dev, devno, 1);
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