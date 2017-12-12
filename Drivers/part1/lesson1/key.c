#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/io.h>

#define GPNCON	0x7f008830

static irqreturn_t key_int(int irq, void *dev_id)
{
	/*Check if a key interrupt has occurred */
	
	/*Clear key interrupts that have occurred(If it is a CPU internal interrupt (non-peripheral), the system will help clear) */
	
	printk(KERN_WARNING"key down!\n");
	//printf("key down!\n");
	
	return 0;
}

void key_hw_init(void)
{
	unsigned int data;
	unsigned int * gpio_config;
	
	gpio_config = ioremap(GPNCON, 4);
	data = readl(gpio_config);
	data &= ~0b11;					//set key1
	data |= 0b10;
	writel(data, gpio_config);
}

int key_open(struct inode * node, struct file * filp)
{
	return 0;
}


struct file_operations key_fops = {
	.open = key_open,
};


struct miscdevice key_miscdev = {
	.minor = 200,
	.name = "key",
	.fops = &key_fops,
};

static int key_init(void)
{
	printk(KERN_WARNING"key init\n");
	misc_register(&key_miscdev);
	
	key_hw_init();
	
	request_irq(S3C_EINT(0), key_int, IRQF_TRIGGER_FALLING, "key", 0);
	
	return 0;
}

static void key_exit(void)
{
	printk(KERN_WARNING"key exit\n");
	misc_deregister(&key_miscdev);
	free_irq(S3C_EINT(0), 0);
}


module_init(key_init);
module_exit(key_exit);
MODULE_LICENSE("GPL");