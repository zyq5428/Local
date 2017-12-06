#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <mach/gpio-bank-k.h>
#include "led.h"

#define LEDCON 0x7f008820
#define LEDDAT 0x7f008824
unsigned int *led_config; 
unsigned int *led_data; 

struct cdev cdev;
dev_t devno;

int led_open(struct inode *node, struct file *filp)
{
	led_config = ioremap(LEDCON,4);
	writel(0x00001111,led_config);
	
	led_data = ioremap(LEDDAT,4);
	
	return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
	    case LED_ON:
	        writel(0x00,led_data);
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

static int led_init()
{
    cdev_init(&cdev,&led_fops);
    
    alloc_chrdev_region(&devno, 0 , 1 , "myled");
    cdev_add(&cdev, devno, 1);
    
    return 0;	
}

static void led_exit()
{
	cdev_del(&cdev);
	unregister_chrdev_region(devno,1);
}


module_init(led_init);
module_exit(led_exit);
